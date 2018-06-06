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
		rules: [{ background: 'fafafa' }],
		colors: {
			"foreground": "#85888c",
			"scrollbar.shadow": "#7e848c11",
			"selection.background": "#f0eee4fd",
			"editor.background": "#fafafa",
			"editor.foreground": "#5c6066",
			"editor.selectionBackground": "#f0eee4",
			"editor.findMatchBackground": "#ff6a0033",
			"editor.findMatchHighlightBackground": "#ff6a0033",
			"editor.findRangeHighlightBackground": "#ff6a0033",
			"editor.lineHighlightBackground": "#f2f2f2",
			"editor.rangeHighlightBackground": "#f2f2f2",
			"editor.wordHighlightBackground": "#ff6a0033",
			"editor.wordHighlightStrongBackground": "#ff6a0033",
			"editorCursor.foreground": "#ff6a00",
			"editorWhitespace.foreground": "#d9d8d7",
			"editorIndentGuide.background": "#d9d8d7",
			"editorLineNumber.foreground": "#b20000",
			"editorSuggestWidget.background": "#ffffff",
			"editorSuggestWidget.border": "#e2e4e7",
			"editorSuggestWidget.selectedBackground": "#f5f5f5",
			"editorSuggestWidget.highlightForeground": "#ff6a00",
			"editorRuler.foreground": "#e2e4e7",
			"editorOverviewRuler.border": "#e2e4e7"
		}
	});

	monaco.editor.setTheme('lightTheme');

	var editor = monaco.editor.create(document.getElementById('editor'), {
				contextmenu: false,
				quickSuggestions: false,
				automaticLayout: true,
				model : model,
				fontSize: 14,
				folding: true,
				formatOnPaste: true
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
			ucleTabs.changeEditorLanguage(ucleTabs.currentTab, "");
			initType = true;
		}
	});

	editor.onDidFocusEditor(function(e) {
		document.getElementById('line-column-count').innerHTML = "Line: " + editor.getPosition().lineNumber + " | Column: " + editor.getPosition().column;
	});

	editor.onDidChangeCursorSelection(function(e) {
		if(e.selection.startLineNumber == e.selection.endLineNumber && e.selection.startColumn == e.selection.endColumn) {
			document.getElementById('line-column-count').innerHTML = "Line: " + e.selection.startLineNumber + " | Column: " + e.selection.startColumn;
		} else if((e.selection.startLineNumber == e.selection.endLineNumber) && (e.selection.startColumn != e.selection.endColumn)) {
			document.getElementById('line-column-count').innerHTML = (e.selection.endColumn - e.selection.startColumn) + " characters selected";
																	 (e.selection.endLineNumber - e.selection.startLineNumber) + " characters selected";
		} else {
			document.getElementById('line-column-count').innerHTML = (e.selection.endLineNumber - e.selection.startLineNumber + 1) + " lines selected | " +
																	 editor.getModel().getValueLength() + " characters selected";
		}
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
		listedFiles.style.height = parseInt(listedFiles.style.height, 10) + parseInt(styleAdded.height, 10) + 7 + "px";

		folder.parentNode.removeChild(folder);

		if(!document.querySelector('[id^="open-"')) {
			explorer.children[0].className = "explorer-ico-closed";
			listedFiles.style.height = "86%";
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
		var styleListed = window.getComputedStyle(listedFiles , null);
		listedFiles.style.height = Math.ceil(parseInt(styleListed.height, 10) - parseInt(styleAdded.height, 10)) - 7 + "px";
	});

	el.addEventListener('tabClose', function(e) {
		var currFileName = e.detail.tabEl.querySelector('.ucle-tab-title').textContent;
		var currFilePath = e.detail.tabEl.querySelector('.ucle-tab-file-path').textContent;
		if(currFileName.substring(0,8) == 'untitled') {
			fileManager.saveAsFile();
		} else {
			fileManager.saveFile(currFilePath);
		}
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
