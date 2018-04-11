const Draggabilly = require('draggabilly');
const ipcRenderer = require('electron').ipcRenderer;

var newTab;
var tabId = new Map();
var idModels = new Map();
var idView = new Map();

(function(){
	const tabTemplate = `
		<div class="ucle-tab">
			<div class="ucle-tab-background">
				<svg version="1.1" xmlns="http://www.w3.org/2000/svg"><defs><symbol id="topleft" viewBox="0 0 214 29" ><path d="M14.3 0.1L214 0.1 214 29 0 29C0 29 12.2 2.6 13.2 1.1 14.3-0.4 14.3 0.1 14.3 0.1Z"/></symbol><symbol id="topright" viewBox="0 0 214 29"><use xlink:href="#topleft"/></symbol><clipPath id="crop"><rect class="mask" width="100%" height="100%" x="0"/></clipPath></defs><svg width="50%" height="100%" transfrom="scale(-1, 1)"><use xlink:href="#topleft" width="214" height="29" class="ucle-tab-background"/><use xlink:href="#topleft" width="214" height="29" class="ucle-tab-shadow"/></svg><g transform="scale(-1, 1)"><svg width="50%" height="100%" x="-100%" y="0"><use xlink:href="#topright" width="214" height="29" class="ucle-tab-background"/><use xlink:href="#topright" width="214" height="29" class="ucle-tab-shadow"/></svg></g></svg>
			</div>
			<div class="ucle-tab-favicon"></div>
			<div class="ucle-tab-title"></div>
			<div class="ucle-tab-close"></div>
		</div>
	`

	const defaultTapProperties = {
		title: '',
		favicon: ''
	}

	let instanceId = 0
	let id = 0;

	class UCLETabs {
		constructor({ editor, monaco}) {
			this.editor = editor;
			this.monaco = monaco;
			this.draggabillyInstances = [];
		}

		init(el, options) {
			this.el = el
			this.options = options

			this.instanceId = instanceId
			this.el.setAttribute('data-ucle-tabs-instance-id', this.instanceId)
			instanceId += 1

			this.setupStyleEl()
			this.setupEvents()
			this.layoutTabs()
			this.fixZIndexes()
			this.setupDraggabilly()

			ipcRenderer.on('close-file', (e) => {
				if(el.querySelector('.ucle-tab-current') !== null) 
					this.removeTab(el.querySelector('.ucle-tab-current'))
			});

			ipcRenderer.on('new-file', (e) => {
				this.addTab();
			})

			ipcRenderer.on('open-file', (e, file) => {
				this.addTab({title: this.getFileName(file)}, true);
			})

			ipcRenderer.on('save-file', (e, file) => {
				if(file) this.addTab({title: this.getFileName(file)}, true);
			})
		}

		getFileName (str) {
			return str.split('\\').pop().split('/').pop();
		}

		emit(eventName, data) {
			this.el.dispatchEvent(new CustomEvent(eventName, { detail: data }))
		}

		setupStyleEl() {
			this.animationStyleEl = document.createElement('style')
			this.el.appendChild(this.animationStyleEl)
		}

		setupEvents() {
			window.addEventListener('resize', event => this.layoutTabs())

			this.el.addEventListener('dblclick', event => this.addTab())

			this.el.addEventListener('click', ({target}) => {
				if (target.classList.contains('ucle-tab')) {
					this.setCurrentTab(target)
				} else if (target.classList.contains('ucle-tab-close')) {
					this.removeTab(target.parentNode)
				} else if (target.classList.contains('ucle-tab-title') || target.classList.contains('ucle-tab-favicon')) {
					this.setCurrentTab(target.parentNode)
				}
			})
		}

		get tabEls() {
			return Array.prototype.slice.call(this.el.querySelectorAll('.ucle-tab'))
		}

		get tabContentEl() {
			return this.el.querySelector('.ucle-tabs-content')
		}

		get tabWidth() {
			const tabsContentWidth = this.tabContentEl.clientWidth - this.options.tabOverlapDistance
			const width = (tabsContentWidth / this.tabEls.length) + this.options.tabOverlapDistance
			return Math.max(this.options.minWidth, Math.min(this.options.maxWidth, width))
		}

		get tabEffectiveWidth() {
			return this.tabWidth - this.options.tabOverlapDistance
		}

		get tabPositions() {
			const tabEffectiveWidth = this.tabEffectiveWidth
			let left = 0
			let positions = []

			this.tabEls.forEach((tabEl, i) => {
				positions.push(left)
				left += tabEffectiveWidth
			})
			return positions
		}

		layoutTabs() {
			const tabWidth = this.tabWidth

			this.cleanUpPreviouslyDraggedTabs()
			this.tabEls.forEach((tabEl) => tabEl.style.width = tabWidth + 'px')
			requestAnimationFrame(() => {
				let styleHTML = ''
				this.tabPositions.forEach((left, i) => {
					styleHTML += `
						.ucle-tabs[data-ucle-tabs-instance-id="${ this.instanceId }"] .ucle-tab:nth-child(${ i + 1 }) {
							transform: translate3d(${ left }px, 0, 0)
						}
					`
				})
				this.animationStyleEl.innerHTML = styleHTML
			})
		}

		fixZIndexes() {
			const bottomBarEl = this.el.querySelector('.ucle-tabs-bottom-bar')
			const tabEls = this.tabEls

			tabEls.forEach((tabEl, i) => {
				let zIndex = tabEls.length - i

				if (tabEl.classList.contains('ucle-tab-current')) {
					bottomBarEl.style.zIndex = tabEls.length + 1
					zIndex = tabEls.length + 2
				}
				tabEl.style.zIndex = zIndex
			})
		}

		createNewTabEl() {
			const div = document.createElement('div')
			div.innerHTML = tabTemplate
			return div.firstElementChild
		}

		addTab(tabProperties, start = false) {
			const tabEl = this.createNewTabEl()

			tabEl.classList.add('ucle-tab-just-added')
			setTimeout(() => tabEl.classList.remove('ucle-tab-just-added'), 500)

			tabProperties = Object.assign({}, defaultTapProperties, tabProperties)
			this.tabContentEl.appendChild(tabEl)
			this.updateTab(tabEl, tabProperties)
			this.emit('tabAdd', { tabEl })

			tabId.set(tabEl, id)
			if(start == true) {
				idModels.set(id, this.editor.getModel())
			} else {
				idModels.set(id, this.monaco.editor.createModel(""))
			}
			idView.set(id, this.editor.saveViewState())
			id++

			this.setCurrentTab(tabEl)
			this.layoutTabs()
			this.fixZIndexes()
			this.setupDraggabilly()
		}

		setCurrentTab(tabEl) {
			const currentTab = this.el.querySelector('.ucle-tab-current')

			if (currentTab) {
				idView.set(tabId.get(currentTab), this.editor.saveViewState())
				currentTab.classList.remove('ucle-tab-current')
			}

			tabEl.classList.add('ucle-tab-current')
			this.fixZIndexes()
			this.emit('activeTabChange', { tabEl })

			this.editor.setModel(idModels.get(tabId.get(tabEl)));
			this.editor.restoreViewState(idView.get(tabId.get(tabEl)));
			this.editor.focus();
		}

		removeTab(tabEl) {
			if (tabEl.classList.contains('ucle-tab-current')) {
				if (tabEl.previousElementSibling) {
					this.setCurrentTab(tabEl.previousElementSibling)
				} else if (tabEl.nextElementSibling) {
					this.setCurrentTab(tabEl.nextElementSibling)
				}
			}

			var removedId = tabId.get(tabEl)

			tabId.forEach(function(value, key) {
				if(tabId.get(key) > removedId)
					tabId.set(key, tabId.get(key)-1)
			});
			tabId.delete(tabEl)

			idModels.get(removedId).dispose()
			idModels.forEach(function(value, key) {
				if(key >= removedId && (key+1 < idModels.size))
					idModels.set(key, idModels.get(key+1))
			});
			idView.forEach(function(value, key) {
				if(key >= removedId && (key+1 < idView.size))
					idView.set(key, idView.get(key+1))
			});

			id--;

			if(!tabEl.previousElementSibling && !tabEl.nextElementSibling) {
				this.editor.setModel(this.monaco.editor.createModel(""))
			}

			tabEl.parentNode.removeChild(tabEl)
			this.emit('tabRemove', { tabEl })
			this.layoutTabs()
			this.fixZIndexes()
			this.setupDraggabilly()
		}

		updateTab(tabEl, tabProperties) {
			tabEl.querySelector('.ucle-tab-title').textContent = tabProperties.title
			tabEl.querySelector('.ucle-tab-favicon').style.backgroundImage = `url('${tabProperties.favicon}')`
		}

		cleanUpPreviouslyDraggedTabs() {
			this.tabEls.forEach((tabEl) => tabEl.classList.remove('ucle-tab-just-dragged'))
		}

		setupDraggabilly() {
			const tabEls = this.tabEls
			const tabEffectiveWidth = this.tabEffectiveWidth
			const tabPositions = this.tabPositions

			this.draggabillyInstances.forEach(draggabillyInstance => draggabillyInstance.destroy())

			tabEls.forEach((tabEl, originalIndex) => {
				const originalTabPositionX = tabPositions[originalIndex]
				const draggabillyInstance = new Draggabilly(tabEl, {
					axis: 'x',
					containment: this.tabContentEl
				})

				this.draggabillyInstances.push(draggabillyInstance)

				draggabillyInstance.on('dragStart', () => {
					this.cleanUpPreviouslyDraggedTabs()
					tabEl.classList.add('ucle-tab-currently-dragged')
					this.el.classList.add('ucle-tabs-sorting')
					this.fixZIndexes()
				})

				draggabillyInstance.on('dragEnd', () => {
					const finalTranslateX = parseFloat(tabEl.style.left, 10)
					tabEl.style.transform = `translate3d(0, 0, 0)`

					// Animate dragged tab back into its place
					requestAnimationFrame(() => {
						tabEl.style.left = '0'
						tabEl.style.transform = `translate3d(${ finalTranslateX }px, 0, 0)`

						requestAnimationFrame(() => {
							tabEl.classList.remove('ucle-tab-currently-dragged')
							this.el.classList.remove('ucle-tabs-sorting')

							this.setCurrentTab(tabEl)
							tabEl.classList.add('ucle-tab-just-dragged')

							requestAnimationFrame(() => {
								tabEl.style.transform = ''

								this.setupDraggabilly()
							})
						})
					})
				})

				draggabillyInstance.on('dragMove', (event, pointer, moveVector) => {
					// Current index be computed within the event since it can change during the dragMove
					const tabEls = this.tabEls
					const currentIndex = tabEls.indexOf(tabEl)

					const currentTabPositionX = originalTabPositionX + moveVector.x
					const destinationIndex = Math.max(0, Math.min(tabEls.length, Math.floor((currentTabPositionX + (tabEffectiveWidth / 2)) / tabEffectiveWidth)))

					if (currentIndex !== destinationIndex) {
						this.animateTabMove(tabEl, currentIndex, destinationIndex)
					}
				})
			})
		}

		animateTabMove(tabEl, originIndex, destinationIndex) {
			if (destinationIndex < originIndex) {
				tabEl.parentNode.insertBefore(tabEl, this.tabEls[destinationIndex])
			} else {
				tabEl.parentNode.insertBefore(tabEl, this.tabEls[destinationIndex + 1])
			}
		}
	}

	module.exports = UCLETabs
})()
