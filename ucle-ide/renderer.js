const loader = require('monaco-loader');
const { remote } = require('electron');
const ipcRenderer = require('electron').ipcRenderer;
const FileManager = require('./scripts/file_manage');
const UCLETabs = require('./scripts/tabs')

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

	const fileManager = new FileManager({ editor, monaco });

	const ucleTabs = new UCLETabs({ editor, monaco });
	ucleTabs.init(el, { tabOverlapDistance: 14, minWidth: 45, maxWidth: 243 });

	editor.onDidChangeModelContent(function(e) {
		if(!initType && !el.querySelector('.ucle-tab-current')) {
			ucleTabs.addTab(null, true);
			initType = true;
		}
	});

	editor.onDidFocusEditor(function(e) {
		document.getElementById('line-tab').innerHTML = "Line: " + editor.getPosition().lineNumber + " | Column: " + editor.getPosition().column
	})

	editor.onDidChangeCursorPosition(function(e) {
		document.getElementById('line-tab').innerHTML = "Line: " + editor.getPosition().lineNumber + " | Column: " + editor.getPosition().column
	})

	el.addEventListener('tabRemove', function(e) {
		if (!el.querySelector('.ucle-tab-current')) {
			initType = false;
		}
	})

	el.addEventListener('tabClose', function(e) {
		fileManager.saveFile(e.detail.tabEl.querySelector('.ucle-tab-title').textContent)
	})

	ipcRenderer.on('save-file', (e) => {
		if(fileManager.f != "") ucleTabs.updateTab(el.querySelector('.ucle-tab-current'), {title: ucleTabs.getFileName(fileManager.f)});
	})

	fileManager.readFolder('/');

	editor.focus()
});