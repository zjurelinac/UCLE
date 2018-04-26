const loader = require('monaco-loader');
const { remote, ipcRenderer } = require('electron');
const path = require('path');
const Menu = remote.Menu;
const MenuItem = remote.MenuItem;

const FileManager = require('./scripts/file_manage');
const UCLETabs = require('./scripts/tabs');
let quickTools, friscAssembly;

loader().then((monaco) => {
	var initType = false;
	var el = document.querySelector('.ucle-tabs');
	var model = monaco.editor.createModel("");

	/*const ctxMenu = new Menu();

	ctxMenu.append(new MenuItem({
		label: 'HELLO'
	}));*/

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
				model : model
	});

	const fileManager = new FileManager({ editor, monaco });

	const ucleTabs = new UCLETabs({ editor, monaco });
	ucleTabs.init(el, { tabOverlapDistance: 14, minWidth: 45, maxWidth: 243 });

	editor.onDidChangeModelContent(function(e) {
		if(!initType && !ucleTabs.currentTab) {
			ucleTabs.addTab(null, false);
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

	quickTools = require("./scripts/quick_tools")(fileManager,ucleTabs);
	friscAssembly = require("./scripts/frisc_language")(monaco);

	/*document.getElementById("quick-tools").addEventListener("contextmenu", function(e) {
		ctxMenu.popup(remote.getCurrentWindow(), { x: e.x, y: e.y})
	});*/

	editor.focus();
});
