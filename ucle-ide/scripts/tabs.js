const Draggabilly = require('draggabilly');
const ipcRenderer = require('electron').ipcRenderer;
const { remote } = require('electron');
const mime = require('mime');

var newTab;
var tabId = new Map();
var idModels = new Map();
var idView = new Map();
var idFile = new Map();
var fileStartContent = new Map();

const tabTemplate = `
	<div class="ucle-tab">
		<div class="ucle-tab-background">
			<svg version="1.1" xmlns="http://www.w3.org/2000/svg"><defs><symbol id="topleft" viewBox="0 0 214 29" ><path d="M14.3 0.1L214 0.1 214 29 0 29C0 29 12.2 2.6 13.2 1.1 14.3-0.4 14.3 0.1 14.3 0.1Z"/></symbol><symbol id="topright" viewBox="0 0 214 29"><use xlink:href="#topleft"/></symbol><clipPath id="crop"><rect class="mask" width="100%" height="100%" x="0"/></clipPath></defs><svg width="50%" height="100%" transfrom="scale(-1, 1)"><use xlink:href="#topleft" width="214" height="29" class="ucle-tab-background"/><use xlink:href="#topleft" width="214" height="29" class="ucle-tab-shadow"/></svg><g transform="scale(-1, 1)"><svg width="50%" height="100%" x="-100%" y="0"><use xlink:href="#topright" width="214" height="29" class="ucle-tab-background"/><use xlink:href="#topright" width="214" height="29" class="ucle-tab-shadow"/></svg></g></svg>
		</div>
		<div class="ucle-tab-favicon"></div>
		<div class="ucle-tab-title"></div>
		<div class="ucle-tab-file-path" hidden></div>
		<div class="ucle-tab-close"></div>
	</div>
`;

let instanceId = 0;
let id = 0;
let untitledCount = 1;

var defaultTapProperties = {
	title: 'untitled-'+untitledCount,
	favicon: '',
	fullPath: 'untitled-'+untitledCount
};

class UCLETabs {
	constructor({ editor, monaco }) {
		this.editor = editor;
		this.monaco = monaco;
		this.draggabillyInstances = [];
	}

	init(el, options) {
		this.el = el;
		this.options = options;
		this.simRunning = false;

		this.instanceId = instanceId;
		this.el.setAttribute('data-ucle-tabs-instance-id', this.instanceId);
		instanceId += 1;

		this.setupStyleEl();
		this.setupEvents();
		this.layoutTabs();
		this.fixZIndexes();
		this.setupDraggabilly();

		ipcRenderer.on('close-file', (e) => {
			var changed = this.checkIfValueChanged(this.currentTab);
			this.closeTab(this.currentTab, changed);
		});

		ipcRenderer.on('close-all-files',  (e) => {
			this.closeAllTabs();
		});

		ipcRenderer.on('new-file', (e) => {
			this.addTab();
		})

		ipcRenderer.on('open-file', (e, file) => {
			this.addTab({title: this.getFileName(file), fullPath: file}, true);
		})

	}

	emit(eventName, data) {
		this.el.dispatchEvent(new CustomEvent(eventName, { detail: data }));
	}

	hideTabs() {
		let tabs = this.tabEls;
		tabs.forEach(function(tab) {
			if(tab != this.currentTab) {
				tab.style.display = "none";
			}
		}, this);
	}

	showAllTabs() {
		let tabs = this.tabEls;
		tabs.forEach(function(tab) {
			if(tab != this.currentTab) {
				tab.style.display = "inherit";
			}
		}, this);
	}

	changeEditorLanguage(tabEl, fileName) {
		if(!tabEl || !idModels.get(tabId.get(tabEl))) return;
		var languageName = this.getExtension(fileName);
		if(languageName == "js") {
			languageName = "javascript";
		} else if(languageName == "s" || !languageName) {
			languageName = "frisc-assembly";
		}
		this.monaco.editor.setModelLanguage(idModels.get(tabId.get(tabEl)),languageName);
	}

	closeTab(tabEl, changed = false) {
		if(this.simRunning) {
			var type = "warning";
			var buttons = ['OK'];
			var message = 'Cannot close file while simulation is running!';
			var defaultId = 0;
			var title = "Warning"
			remote.dialog.showMessageBox({message, type, buttons, defaultId, title});
			return;
		} else if(tabEl) {
			if(changed) {
				var type = "warning";
				var buttons = ['Cancel','Close file without saving','Save'];
				var message = 'Save changes before closing?';
				var defaultId = 2;
				var title = "Warning"
				var response = remote.dialog.showMessageBox({message, type, buttons, defaultId, title});

				if(response == 2) {
					this.emit('tabClose', { tabEl });
				} else if(response == 0) {
					return;
				}
			}
			this.removeTab(tabEl);
		}
	}

	closeAllTabs() {
		if(this.simRunning) return;

		var allTabs = this.tabEls;
		allTabs.forEach(function(tab) {
			var changed = this.checkIfValueChanged(tab);
			if(changed) {
				this.setCurrentTab(tab);
				setTimeout(() => this.closeTab(tab, changed), 50);
			} else {
				this.closeTab(tab, changed);
			}
		}, this)
	}

	setupStyleEl() {
		this.animationStyleEl = document.createElement('style');
		this.el.appendChild(this.animationStyleEl);
	}

	setupEvents() {
		window.addEventListener('resize', event => this.layoutTabs());

		this.el.addEventListener('dblclick', event => this.addTab());

		this.el.addEventListener('click', ({target}) => {
			if (target.classList.contains('ucle-tab')) {
				this.setCurrentTab(target);
			} else if (target.classList.contains('ucle-tab-close')) {
				var changed = this.checkIfValueChanged(target.parentNode);
				this.closeTab(target.parentNode, changed);
			}
		})
	}

	getFileName(str) {
		if(!str) return;
		return str.split('\\').pop().split('/').pop();
	}

	getExtension(path) {
		return mime.getExtension(mime.getType(path))
	}

	setCurrentTabByPath(path) {
		var allTabs = this.tabEls;
		allTabs.forEach(function(tab) {
			if(tab.querySelector('.ucle-tab-file-path').textContent == path) {
				this.setCurrentTab(tab);
				return;
			}
		}, this);
	}

	closeTabByPath(path) {
		var allTabs = this.tabEls;
		allTabs.forEach(function(tab) {
			if(tab.querySelector('.ucle-tab-file-path').textContent == path) {
				var changed = this.checkIfValueChanged(tab);
				this.closeTab(tab, changed);
				return;
			}
		}, this);
	}

	get firstFreeUntitled() {
		var allTabs = this.tabEls;
		var currMax = 0;
		for(var i = 0; i < allTabs.length; i++) {
			var title = allTabs[i].querySelector('.ucle-tab-title').textContent;
			if(title.includes("untitled-")) {
				var untitledNumber = parseInt(title.replace("untitled-",""));
				if((untitledNumber < untitledCount) && (currMax < untitledNumber)) {
					currMax = untitledNumber;
				}
			}
		}
		return currMax+1;
	}

	get currentTabValue() {
		var currTab = this.el.querySelector('.ucle-tab-current');
		if(currTab && idModels.get(tabId.get(currTab))) {
			return idModels.get(tabId.get(currTab)).getValue();
		} else {
			return currTab;
		}
	}

	get currentTabModel() {
		var currTab = this.el.querySelector('.ucle-tab-current');
		if(currTab && idModels.get(tabId.get(currTab))) {
			return idModels.get(tabId.get(currTab));
		} else {
			return null;
		}
	}

	get currentTab() {
		return this.el.querySelector('.ucle-tab-current');
	}

	get tabEls() {
		return Array.prototype.slice.call(this.el.querySelectorAll('.ucle-tab'));
	}

	get tabContentEl() {
		return this.el.querySelector('.ucle-tabs-content');
	}

	get tabWidth() {
		const tabsContentWidth = this.tabContentEl.clientWidth - this.options.tabOverlapDistance;
		const width = (tabsContentWidth / this.tabEls.length) + this.options.tabOverlapDistance;
		return Math.max(this.options.minWidth, Math.min(this.options.maxWidth, width));
	}

	get tabEffectiveWidth() {
		return this.tabWidth - this.options.tabOverlapDistance;
	}

	get tabPositions() {
		const tabEffectiveWidth = this.tabEffectiveWidth;
		let left = 0;
		let positions = [];

		this.tabEls.forEach((tabEl, i) => {
			positions.push(left);
			left += tabEffectiveWidth;
		})
		return positions;
	}

	layoutTabs() {
		const tabWidth = this.tabWidth;

		this.cleanUpPreviouslyDraggedTabs();
		this.tabEls.forEach((tabEl) => tabEl.style.width = tabWidth + 'px');
		requestAnimationFrame(() => {
			let styleHTML = '';
			this.tabPositions.forEach((left, i) => {
				styleHTML += `
					.ucle-tabs[data-ucle-tabs-instance-id="${ this.instanceId }"] .ucle-tab:nth-child(${ i + 1 }) {
						transform: translate3d(${ left }px, 0, 0)
					}
				`;
			})
			this.animationStyleEl.innerHTML = styleHTML;
		})
	}

	fixZIndexes() {
		const bottomBarEl = this.el.querySelector('.ucle-tabs-bottom-bar');
		const tabEls = this.tabEls;

		tabEls.forEach((tabEl, i) => {
			let zIndex = tabEls.length - i;

			if (tabEl.classList.contains('ucle-tab-current')) {
				bottomBarEl.style.zIndex = tabEls.length + 1;
				zIndex = tabEls.length + 2;
			}
			tabEl.style.zIndex = zIndex;
		})
	}

	createNewTabEl() {
		const div = document.createElement('div');
		div.innerHTML = tabTemplate;
		return div.firstElementChild;
	}

	checkIfValueChanged(tabEl) {
		if(!tabEl) return;
		return (idModels.get(tabId.get(tabEl)).getValue() != fileStartContent.get(idFile.get(tabId.get(tabEl))));
	}

	checkIfAlreadyOpen(tabProperties) {
		if(!tabProperties) return false;

		var openedFileId = null;

		if(idFile.size === 0) return false;

		idFile.forEach(function(value, key) {
			if(value === tabProperties.title) {
				openedFileId = key; 
				return;
			}
		})

		if(openedFileId === undefined || openedFileId === null) {
			return false;
		}

		const currentTab = this.currentTab;

		if(tabId.get(currentTab) != openedFileId) {
			var changeTab;
			tabId.forEach(function(value,key) {
				if(value === openedFileId) {
					changeTab = key;
					return;
				}
			})
			if(changeTab) this.setCurrentTab(changeTab);
		}
		return true;
	}
	
	checkIfTabOpened(path) {
		return this.tabEls.some(function(tab) {
			if(tab.querySelector('.ucle-tab-file-path').textContent == path) {
				return true;
			}
		});
	}

	getTabByPath(path) {
		var openedTab = null;
		this.tabEls.forEach(function(tab) {
			if(tab.querySelector('.ucle-tab-file-path').textContent == path) {
				openedTab = tab;
				return;
			}
		});
		return openedTab;
	}

	addTab(tabProperties, start = false) {
		if(this.checkIfAlreadyOpen(tabProperties) || this.simRunning) return;

		const tabEl = this.createNewTabEl();
		var isBlankTab = !tabProperties;

		tabEl.classList.add('ucle-tab-just-added');
		setTimeout(() => tabEl.classList.remove('ucle-tab-just-added'), 500);

		tabId.set(tabEl, id);

		tabProperties = Object.assign({}, defaultTapProperties, tabProperties);

		this.tabContentEl.appendChild(tabEl);
		this.updateTab(tabEl, tabProperties);

		if(start == true) {
			idModels.set(id, this.editor.getModel());
			if(isBlankTab) {
				fileStartContent.set(idFile.get(tabId.get(tabEl)), "");
			} else {
				fileStartContent.set(idFile.get(tabId.get(tabEl)),this.editor.getModel().getValue());
			}
		} else {
			untitledCount++;
			defaultTapProperties.title = 'untitled-'+untitledCount;
			defaultTapProperties.fullPath = defaultTapProperties.title;
			idModels.set(id, this.monaco.editor.createModel(""));
			fileStartContent.set(idFile.get(tabId.get(tabEl)), "");			
		}

		var tabs = this;

		idModels.get(tabId.get(tabEl)).onDidChangeContent(function(e) {
			if(tabs.checkIfValueChanged(tabEl)) {
				if(tabEl.querySelector(".ucle-tab-close")) {
					tabEl.querySelector(".ucle-tab-close").className = 'ucle-tab-content-changed';
				}
			} else {
				tabEl.querySelector(".ucle-tab-content-changed").className = 'ucle-tab-close';
			}
		});

		tabEl.children[4].addEventListener("mouseover", function(e) {
			if(e.target.matches("div.ucle-tab-content-changed")) {
				e.target.className = 'ucle-tab-close';
			}
		});

		tabEl.children[4].addEventListener("mouseout", function(e) {
			if(e.target.matches("div.ucle-tab-close") && tabs.checkIfValueChanged(tabEl)) {
				e.target.className = 'ucle-tab-content-changed';
			}
		});

		this.changeEditorLanguage(tabEl, tabProperties.title);

		idView.set(id, this.editor.saveViewState());
		id++;

		this.emit('tabAdd', { tabEl });
		this.setCurrentTab(tabEl);
		this.layoutTabs();
		this.fixZIndexes();
		this.setupDraggabilly();
	}

	setCurrentTab(tabEl) {
		if(this.simRunning) return;
		
		const currentTab = this.currentTab;

		if (currentTab) {
			idView.set(tabId.get(currentTab), this.editor.saveViewState());
			currentTab.classList.remove('ucle-tab-current');
		}

		tabEl.classList.add('ucle-tab-current');
		this.fixZIndexes();
		this.layoutTabs();

		this.emit('activeTabChange', { tabEl });

		this.editor.setModel(idModels.get(tabId.get(tabEl)));
		this.editor.restoreViewState(idView.get(tabId.get(tabEl)));
		this.editor.focus();
	}

	removeTab(tabEl) {
		if (tabEl.classList.contains('ucle-tab-current')) {
			if (tabEl.previousElementSibling) {
				this.setCurrentTab(tabEl.previousElementSibling);
			} else if (tabEl.nextElementSibling) {
				this.setCurrentTab(tabEl.nextElementSibling);
			}
		}

		if(!tabEl.previousElementSibling && !tabEl.nextElementSibling) {
			this.editor.setModel(this.monaco.editor.createModel(""));
		}

		var removedId = tabId.get(tabEl);

		tabId.delete(tabEl);

		tabId.forEach(function(value, key) {
			if(tabId.get(key) > removedId)
				tabId.set(key, tabId.get(key)-1);
		});

		idModels.get(removedId).dispose();
		idModels.forEach(function(value, key) {
			if(key >= removedId && (key+1 < idModels.size))
				idModels.set(key, idModels.get(key+1));
		});
		idView.forEach(function(value, key) {
			if(key >= removedId && (key+1 < idView.size))
				idView.set(key, idView.get(key+1));
		});

		idFile.forEach(function(value, key) {
			if(key > removedId && (key-1 >= 0)) {
				idFile.set(key-1, value);
			}
		});

		idFile.delete(idFile.size-1);

		fileStartContent.forEach(function(value, key) {
			if(key > removedId && (key-1 >= 0)) {
				fileStartContent.set(key-1, value);
			}
		});

		fileStartContent.delete(fileStartContent.size-1);

		id--;

		tabEl.parentNode.removeChild(tabEl);

		untitledCount = this.firstFreeUntitled;
		defaultTapProperties.title = 'untitled-'+untitledCount;
		defaultTapProperties.fullPath = defaultTapProperties.title;

		this.emit('tabRemove', { tabEl });
		this.layoutTabs();
		this.fixZIndexes();
		this.setupDraggabilly();
	}

	updateTab(tabEl, tabProperties) {
		tabEl.querySelector('.ucle-tab-title').textContent = tabProperties.title;
		tabEl.querySelector('.ucle-tab-file-path').textContent = tabProperties.fullPath;
		idFile.set(tabId.get(tabEl), tabProperties.title);
	}

	updateTabContent(tabEl) {
		fileStartContent.set(idFile.get(tabId.get(tabEl)),this.editor.getModel().getValue());
		if(tabEl.querySelector(".ucle-tab-content-changed")) {
			tabEl.querySelector(".ucle-tab-content-changed").className = "ucle-tab-close";
		}
	}

	cleanUpPreviouslyDraggedTabs() {
		this.tabEls.forEach((tabEl) => tabEl.classList.remove('ucle-tab-just-dragged'));
	}

	setupDraggabilly() {
		const tabEls = this.tabEls;
		const tabEffectiveWidth = this.tabEffectiveWidth;
		const tabPositions = this.tabPositions;

		this.draggabillyInstances.forEach(draggabillyInstance => draggabillyInstance.destroy());

		tabEls.forEach((tabEl, originalIndex) => {
			const originalTabPositionX = tabPositions[originalIndex]
			const draggabillyInstance = new Draggabilly(tabEl, {
				axis: 'x',
				containment: this.tabContentEl
			});

			this.draggabillyInstances.push(draggabillyInstance);

			draggabillyInstance.on('dragStart', () => {
				this.cleanUpPreviouslyDraggedTabs();
				tabEl.classList.add('ucle-tab-currently-dragged');
				this.el.classList.add('ucle-tabs-sorting');
				this.fixZIndexes();
				this.setCurrentTab(tabEl);
			})

			draggabillyInstance.on('dragEnd', () => {
				const finalTranslateX = parseFloat(tabEl.style.left, 10);
				tabEl.style.transform = `translate3d(0, 0, 0)`;

				requestAnimationFrame(() => {
					tabEl.style.left = '0';
					tabEl.style.transform = `translate3d(${ finalTranslateX }px, 0, 0)`;

					requestAnimationFrame(() => {
						tabEl.classList.remove('ucle-tab-currently-dragged');
						this.el.classList.remove('ucle-tabs-sorting');

						this.setCurrentTab(tabEl);
						tabEl.classList.add('ucle-tab-just-dragged');

						requestAnimationFrame(() => {
							tabEl.style.transform = '';

							this.setupDraggabilly();
						})
					})
				})
			})

			draggabillyInstance.on('dragMove', (event, pointer, moveVector) => {
				const tabEls = this.tabEls;
				const currentIndex = tabEls.indexOf(tabEl);

				const currentTabPositionX = originalTabPositionX + moveVector.x;
				const destinationIndex = Math.max(0, Math.min(tabEls.length, Math.floor((currentTabPositionX + (tabEffectiveWidth / 2)) / tabEffectiveWidth)));

				if (currentIndex !== destinationIndex) {
					this.animateTabMove(tabEl, currentIndex, destinationIndex);
				}
			})
		})
	}

	animateTabMove(tabEl, originIndex, destinationIndex) {
		if (destinationIndex < originIndex) {
			tabEl.parentNode.insertBefore(tabEl, this.tabEls[destinationIndex]);
		} else {
			tabEl.parentNode.insertBefore(tabEl, this.tabEls[destinationIndex + 1]);
		}
	}
}

module.exports = UCLETabs;