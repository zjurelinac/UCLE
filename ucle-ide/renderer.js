const loader = require('monaco-loader');
const { remote } = require('electron');
const ipcRenderer = require('electron').ipcRenderer;
const path = require('path');

const FileManager = require('./scripts/file_manage');
const UCLETabs = require('./scripts/tabs');

loader().then((monaco) => {
	var initType = false;
	var el = document.querySelector('.ucle-tabs');
	var model = monaco.editor.createModel("");

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

	var editor = monaco.editor.create(document.getElementById('container'), {
				contextmenu: false,
				quickSuggestions: false,
				automaticLayout: true,
				model : model
	});

	monaco.editor.setModelLanguage(editor.getModel(), "cpp")

	const fileManager = new FileManager({ editor, monaco });

	const ucleTabs = new UCLETabs({ editor, monaco });
	ucleTabs.init(el, { tabOverlapDistance: 14, minWidth: 45, maxWidth: 243 });

	editor.onDidChangeModelContent(function(e) {
		if(!initType && !ucleTabs.currentTab) {
			ucleTabs.addTab(null, true);
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
			} else {
				fileManager.saveFile(currFilePath);
			}
			ucleTabs.updateTabContent(currTab);
		}
	});

	document.getElementById("listed-files").addEventListener("dblclick", function(e) {
		if (e.target && e.target.matches("li.dir")) {
			fileManager.readFolder(e.target.id);
			ucleTabs.closeAllTabs();
		} else if(e.target && e.target.matches("li.file")) {
			fileManager.openFile(e.target.id);
			ucleTabs.addTab({title: ucleTabs.getFileName(e.target.id), fullPath: e.target.id},true);
		}
	});

	document.getElementById("listed-files").addEventListener("mouseover", function(e) {
		e.preventDefault();
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file"))) {
			e.target.style.cursor = "pointer";
			e.target.style.background = "#e9e9e9"
		}
	});

	document.getElementById("listed-files").addEventListener("mouseout", function(e) {
		e.preventDefault();
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file"))) {
			e.target.style.background = "white";
		}
	});

	document.getElementById("listed-files").addEventListener('mousedown', function(e){
		if (e.detail > 1){
			e.preventDefault();
		}
	});

	document.getElementById("quick-tools").addEventListener('mousedown', function(e){
		if (e.detail > 1){
			e.preventDefault();
		}
	});

	document.getElementById("explorer").addEventListener("click", function(e) {
		fileManager.openDirectory();
		ucleTabs.closeAllTabs();
	});

	editor.focus();
});