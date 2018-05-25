const loader = require('monaco-loader');
const { remote, ipcRenderer } = require('electron');
const path = require('path');

const FileManager = require('./scripts/file_manage');
const UCLETabs = require('./scripts/tabs');
const UCLEServer = require('./scripts/ucle_ide_server')
let quickTools, friscAssembly;

loader().then((monaco) => {
	var initType = false;
	var el = document.querySelector('.ucle-tabs');
	var model = monaco.editor.createModel("");

	friscAssembly = require("./scripts/frisc_language")(monaco);

	monaco.editor.defineTheme('myTheme', {
		base: 'vs',
		inherit: true,
		rules: [{ background: 'EDF9FA' }],
		colors: {
			'editor.background': '#f2f2f2',
			'editor.inactiveSelectionBackground': '#88000015'
		}
	});

	monaco.editor.setTheme('myTheme');

	var editor = monaco.editor.create(document.getElementById('editor'), {
				contextmenu: false,
				quickSuggestions: false,
				automaticLayout: true,
				model : model,
				fontSize: 14
	});

	const fileManager = new FileManager({ editor, monaco });
	const ucleTabs = new UCLETabs({ editor, monaco });
	const ucleServer = new UCLEServer({ editor, monaco });

	ucleTabs.init(el, { tabOverlapDistance: 14, minWidth: 45, maxWidth: 243 });
	ucleServer.init();

	quickTools = require("./scripts/quick_tools")(editor, fileManager,ucleTabs, ucleServer);

	editor.onDidChangeModelContent(function(e) {
		if(!initType && !ucleTabs.currentTab) {
			ucleTabs.addTab(null, false);
			ucleTabs.changeEditorLanguage(ucleTabs.currentTab, ".s");
			initType = true;
		}
	});

	editor.onDidFocusEditor(function(e) {
		document.getElementById('line-tab').innerHTML = "Line: " + editor.getPosition().lineNumber + " | Column: " + editor.getPosition().column;
	});

	editor.onDidChangeCursorPosition(function(e) {
		document.getElementById('line-tab').innerHTML = "Line: " + editor.getPosition().lineNumber + " | Column: " + editor.getPosition().column;
	});

	el.addEventListener('tabRemove', function(e) {
		if (!ucleTabs.currentTab) {
			initType = false;
			editor.focus();
		}
		var explorer = document.getElementById('explorer');
		var removedFilePath = e.detail.tabEl.querySelector('.ucle-tab-file-path').textContent;
		var folder = document.getElementById("open-" + removedFilePath);

		folder.parentNode.removeChild(folder);

		if(!document.querySelector('[id^="open-"')) {
			explorer.children[0].className = "explorer-ico-closed";
		}
	});

	el.addEventListener('tabAdd', function(e) {
		var addedFilePath = e.detail.tabEl.querySelector('.ucle-tab-file-path').textContent;
		var addedFileName = e.detail.tabEl.querySelector('.ucle-tab-title').textContent;

		var explorer = document.getElementById('explorer');
		explorer.children[0].className = "explorer-ico-opened";

		document.getElementById("opened-files").innerHTML += '<div id="open-' + addedFilePath + '" class="file" title="' + 
		                                                     addedFilePath + '"><i class="file-ico" ' +
                                                             'style="margin-left:10px; padding-right: 5px"></i>'+ addedFileName + '</div>';
	});

	el.addEventListener('tabClose', function(e) {
		fileManager.saveFile(e.detail.tabEl.querySelector('.ucle-tab-file-path').textContent);
	});

	ipcRenderer.on('save-file', (e) => {
		var currTab = ucleTabs.currentTab;
		if(currTab) {
			var currFileName = currTab.querySelector('.ucle-tab-title').textContent;
			var currFilePath = currTab.querySelector('.ucle-tab-file-path').textContent;
			if(currFileName == 'untitled') {
				var savedPath = fileManager.saveAsFile();
				ucleTabs.updateTab(currTab, {title: ucleTabs.getFileName(savedPath), fullPath: savedPath});
				ucleTabs.changeEditorLanguage(currTab, ucleTabs.getFileName(savedPath));                
			} else {
				fileManager.saveFile(currFilePath);
			}
			ucleTabs.updateTabContent(currTab);
		}
	});
	
	editor.focus();
});
