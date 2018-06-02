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

	monaco.editor.defineTheme('lightTheme', {
		base: 'vs',
		inherit: true,
		rules: [{ background: 'EDF9FA' }],
		colors: {
			'editor.foreground': '#000000',
			'editor.background': '#f2f2f2',
			'editorCursor.foreground': '#8B0000',
			'editor.lineHighLightBackground': '#0000FF20',
			'editorLineNumber.foreground': '#008800',
			'editor.selectrionBackground': '#88000030',
			'editor.inactiveSelectionBackground': '#88000015'
		}
	});

	monaco.editor.setTheme('lightTheme');

	var editor = monaco.editor.create(document.getElementById('editor'), {
				contextmenu: false,
				quickSuggestions: false,
				automaticLayout: true,
				model : model,
				fontSize: 14,
				folding: true
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
		
		var listedFiles = document.getElementById("listed-files");
		var styleAdded = window.getComputedStyle(folder , null);
		listedFiles.style.height = parseInt(listedFiles.style.height, 10) + parseInt(styleAdded.height, 10) + 9 + "px";

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
		                                                     addedFilePath + '"><i class="file-ico"></i><span class="text-wrapper">'+ addedFileName + '</span></div>';

		var listedFiles = document.getElementById("listed-files");
		var addedFile = document.getElementById("open-" + addedFilePath);
		var styleAdded = window.getComputedStyle(addedFile , null);

		listedFiles.style.height = Math.ceil(parseInt(listedFiles.style.height, 10) - parseInt(styleAdded.height, 10) - 9) + "px";
	});

	el.addEventListener('tabClose', function(e) {
		fileManager.saveFile(e.detail.tabEl.querySelector('.ucle-tab-file-path').textContent);
	});

	ipcRenderer.on('save-file', (e) => {
		var currTab = ucleTabs.currentTab;
		if(currTab) {
			var currFileName = currTab.querySelector('.ucle-tab-title').textContent;
			var currFilePath = currTab.querySelector('.ucle-tab-file-path').textContent;
			if(currFileName.substring(0,8) == 'untitled') {
				var savedPath = fileManager.saveAsFile();
				if(!savedPath) {
					return;
				}
				ucleTabs.updateTab(currTab, {title: ucleTabs.getFileName(savedPath), fullPath: savedPath});
				ucleTabs.changeEditorLanguage(currTab, ucleTabs.getFileName(savedPath));
				var untitledFile = document.getElementById("open-" + currFileName);
				untitledFile.id = "open-" + savedPath;
				untitledFile.children[1].innerHTML = ucleTabs.getFileName(savedPath);
			} else {
				fileManager.saveFile(currFilePath);
			}
			ucleTabs.updateTabContent(currTab);
		}
	});
	
	editor.focus();
});
