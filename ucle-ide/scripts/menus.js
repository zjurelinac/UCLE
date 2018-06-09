const dialog = require('electron').dialog;
const remote = require('electron').remote;

function promptInputAndAdd(args) {
	const parent = args[0];
	const fileManager = args[1];
	const ucleTabs = args[2];
	const index = args[3];
	const path = args[4];
	var fileType = args[5];

	var file = document.createElement("li");
	file.className = fileType;
	file.classList.add("highlight-input");

	var fileName = document.createElement("input");
	fileName.className = "file-display";

	var ico = document.createElement("i");
	ico.className = fileType + '-ico';

	if(fileType == "dir") {
		ico.className = ico.className + '-closed';
	}

	file.appendChild(ico);
	file.appendChild(fileName);
	file.appendChild(fileName);
	parent.parentNode.children[index].appendChild(file);

	let elements = parent.parentNode.children[index].getElementsByTagName("li");

	var parentNode = parent.parentNode.children[index].parentNode.firstChild;

	if(parentNode && parentNode.firstChild.style) {
		for(var i = 0; i < elements.length; i++) {
			var ele = elements[i].getElementsByTagName("i")[0];
			var value = parseInt(parentNode.firstChild.style.marginLeft, 10) + 15 + "px";
			if(ele.style.marginLeft < value) {
				ele.style.marginLeft = value;
			}
		}
	}

	fileName.focus();
	fileName.addEventListener('keyup',function(e){
		alreadyCreated = false;
		if(e.which == 13) this.blur();
	});

	var alreadyCreated = false;

	fileName.addEventListener("blur", function(e) {
		if(fileName.value != "") {
			var newFileName = fileName.value;
			var newFilePath = path + '/' + newFileName;

			var child = document.createElement("span");
			child.innerHTML = newFileName;

			if(fileType == "file") {
				var alreadyCreated = fileManager.checkIfExists(newFilePath);
				if(alreadyCreated) {
					var questionType = "warning";
					var buttons = ['OK'];
					var message = 'That file already exists!\nPlease choose a different name!';
					var defaultId = 0;
					var title = "Warning";					
					remote.dialog.showMessageBox({message, questionType, buttons, defaultId, title});
					setTimeout(()=>{fileName.focus();},0);
					return;
				} else {
					fileManager.saveFile(newFilePath);
					ucleTabs.addTab({title: newFileName, fullPath: newFilePath});
				}
			} else {
				var alreadyCreated = fileManager.createFolder(newFilePath);
				if(alreadyCreated) {
					var questionType = "warning";
					var buttons = ['OK'];
					var message = 'That directory already exists!\nPlease choose a different name!';
					var defaultId = 0;
					var title = "Warning";
					remote.dialog.showMessageBox({message, questionType, buttons, defaultId, title});
					setTimeout(()=>{fileName.focus();},0);
					return;
				} else {
					let childListID = newFilePath + '-list-' + newFilePath;
					var fileList = document.createElement("ul");
					fileList.id = childListID;
					fileList.className = "hide";
					parent.parentNode.children[index].appendChild(fileList);
					file.style.background = "white";
				}
			}
			file.title = newFilePath;
			file.replaceChild(child, fileName);
			fileManager.reloadListedFiles();
		} else {
			parent.parentNode.children[index].removeChild(file);
		}
		file.classList.remove("highlight-input");
	});
}

function closeOpenedFiles(ucleTabs, files) {
	for(var i = 0; i < files.length; i++) {
		if(files[i].matches("ul")) {
			closeOpenedFiles(ucleTabs,files[i].children);
		} else if(files[i].matches("li.file") && document.getElementById("open-" + files[i].title)) {
			ucleTabs.removeTab(ucleTabs.getTabByPath(files[i].title));
		}
	}
}

function deleteFile(file, ucleTabs, fileManager, type) {
	var questionType = "question";
	var buttons = ['No','Yes'];
	var message = "Are you sure you want to delete '" + ucleTabs.getFileName(file.title) + "'?";
	var defaultId = 0;
	var title = "Question";
	var response = remote.dialog.showMessageBox({message, questionType, buttons, defaultId, title});

	if(response) {
		if(type == "file") {
			fileManager.deleteFile(file.title);
			if(ucleTabs.checkIfTabOpened(file.title)) {
				ucleTabs.removeTab(ucleTabs.getTabByPath(file.title));
			}
		} else {
			fileManager.deleteFolder(file.title);
			var index = Array.from(file.parentNode.children).indexOf(file);
			var	files = file.parentNode.children[index+1];
			closeOpenedFiles(ucleTabs, files.children);
			file.parentNode.removeChild(file.parentNode.children[index+1]);
			var explorerListed = document.querySelector('[id="div-' + CSS.escape(file.title) + '"]');
			if(explorerListed) {
				document.getElementById("listed-files").removeChild(explorerListed);
			}
		}
		file.parentNode.removeChild(file);
	}
}

function promptInputAndRename(parent, child, type, ucleTabs, fileManager) {
	var input = document.createElement("input");
	input.value = child.innerHTML;

	input.className = "file-display";
	parent.replaceChild(input, child);

	input.addEventListener('focus',function(e){
		var splitInput = input.value.split(".");
		if(splitInput.length > 1 && splitInput[0] != "") {
			this.setSelectionRange(0, splitInput[0].length);
		} else {
			this.select();
		}
	});

	input.focus();
	input.addEventListener('keyup',function(e){
		if(e.which == 13) this.blur();
	});

	input.addEventListener("blur", function(e) {
		var newName = input.value;
		var newPath = parent.title.replace(/[^\/]*$/, '') + newName;
		var oldName = ucleTabs.getFileName(parent.title);
		var oldPath = parent.title;

		var alreadyRenamed = fileManager.renameFile(oldPath, newPath);

		if(alreadyRenamed) {
			var questionType = "warning";
			var buttons = ['OK'];
			var message = "'" + newName +"' already exists!\nPlease choose a different name!";
			var defaultId = 0;
			var title = "Warning";
			remote.dialog.showMessageBox({message, questionType, buttons, defaultId, title});
			setTimeout(()=>{input.focus();},0);
			return;
		} else if(type == "dir") {
			var index = Array.from(parent.parentNode.children).indexOf(parent);
			var files = parent.parentNode.children[index+1];
			files.id = newPath + '-list-' + newPath;
			var explorerListed = document.querySelector('[id="div-' + CSS.escape(oldPath) + '"]');
			if(explorerListed) {
				explorerListed.children[0].title = newPath;
			}
			var explorerOpened = document.querySelectorAll('[id*="open-' + CSS.escape(oldPath) + '"]');
			for(var i = 0; i < explorerOpened.length; i++) {
				var openedFileName = ucleTabs.getFileName(explorerOpened[i].title);
				var openedNewPath = newPath + '/' + openedFileName;
				ucleTabs.updateTab(ucleTabs.getTabByPath(oldPath+ '/' + openedFileName), {title: openedFileName, fullPath: openedNewPath});			
				ucleTabs.updateTabContent(ucleTabs.getTabByPath(openedNewPath));
				explorerOpened[i].id = "open-" + openedNewPath;
				explorerOpened[i].title = openedNewPath;
			}
		} else if(ucleTabs.checkIfTabOpened(oldPath)) {
			ucleTabs.updateTabContent(ucleTabs.getTabByPath(oldPath));
			console.log(oldPath);
			ucleTabs.updateTab(ucleTabs.getTabByPath(oldPath), {title: newName, fullPath: newPath});
			var opened = document.getElementById("open-" + oldPath);
			opened.id = "open-" + newPath;
			opened.title = newPath;
			opened.children[1].innerHTML = newName;
		}
		child.innerHTML = newName;
		parent.title = newPath;
		parent.replaceChild(child, input);
		setTimeout(() => {fileManager.reloadListedFiles();}, 0);
	});
}


module.exports = {
	contextMain:
		(mainWindow) => {
		
			const webContents = mainWindow.webContents;

			return [
				{
					label: 'File',
					submenu: [
						{
							label: 'New file',
							accelerator: 'CmdOrCtrl+N',
							click ()
							{
								webContents.send('new-file');
							}
						},
						{
							label: 'Open file',
							accelerator: 'CmdOrCtrl+O',				
							click () { 	
								dialog.showOpenDialog({title:"Open a file", properties: ['openFile']}, function(filename) { 
									if(filename !== undefined) webContents.send('open-file', filename.toString());
								}); 
							}
						},
						{
							label: 'Open folder',
							click () {
								webContents.send('open-dir');
							}
						},
						{
							label: 'Save',
							accelerator: 'CmdOrCtrl+S',
							click () {
								webContents.send('save-file');
							}
						},
						{
							label: 'Save as...',
							click () { 
								webContents.send('save-as-file');
							}
						},
						{
							type: 'separator'
						},
						{
							label: 'Close file',
							accelerator: 'CmdOrCtrl+W',
							click () { 
								webContents.send('close-file');
							}
						},
						{
							label: 'Close all files',
							click () { 
								webContents.send('close-all-files');
							}
						},
						{
							type: 'separator'
						},
						{
							label: 'Quit',
							accelerator: 'CmdOrCtrl+Q',
							role: 'close'
						}
					]
				},
				{
					label: 'Edit',
					submenu: [
						{
							role: 'undo'
						},
						{
							role: 'redo'
						},
						{
							type: 'separator'
						},
						{
							role: 'cut'
						},
						{
							role: 'copy'
						},
						{
							role: 'paste'
						},
						{
							role: 'pasteandmatchstyle'
						},
						{
							role: 'delete'
						},
						{
							role: 'selectall'
						}
					]
				},
				{
					label: 'Tools',
					submenu: [
						{
							label: 'Themes',
							submenu: [
								{
									label: "Light theme (default)",
									click () {
										webContents.send("light-theme-change");
									}
								},
								{
									label: "Dark theme",
									click () {
										webContents.send("dark-theme-change")
									}
								}
							]
						}
					]
				},
				{
					label: 'View',
					submenu: [
						{
							label: 'Toggle Developer Tools',
							accelerator: process.platform === 'darwin' ? 'Alt+Command+I' : 'Ctrl+Shift+I',
							click (item, focusedWindow) {
								if (focusedWindow) focusedWindow.webContents.toggleDevTools();
							}
						},
						{
							type: 'separator'
						},
						{
							role: 'resetzoom'
						},
						{
							role: 'zoomin'
						},
						{
							role: 'zoomout'
						},
						{
							type: 'separator'
						}
					]
				},
				{
					label: 'Run',
					submenu: [
						{
							label: 'Run on simulator',
							accelerator: 'CmdOrCtrl+R',
							click () {
								webContents.send('run-simulation');
							}
						}
					]
				},
				{
					role: 'help',
					submenu: [
						{
							label: 'Learn More',
							click () { require('electron').shell.openExternal('https://github.com/zjurelinac/UCLE'); }
						}
					]
				}
			];
		},
	contextWorkspace:
		(fileManager, ucleTabs, e) => {
			function removeFolder(folder) {
				fileManager.removeFolder(folder.title); 
				if(!document.querySelector('[id^="div-"')) {
					document.getElementById('workspace-text').style.display = "inline-block";
					var button = document.getElementById("openbtn");
					document.getElementById("quick-tools").style.whiteSpace = "initial";
					button.style.display = "block";
					button.addEventListener("click", function(e) {
						if(fileManager.openDirectory()) {
							document.getElementById("quick-tools").style.whiteSpace = "nowrap";
						}
					});
				}
			}

			return [
				{
					click() {
						removeFolder(e, fileManager, ucleTabs);
					},
					label: "Remove folder from workspace"
				},
				{
					type: 'separator'					
				},
				{
					click() {
						e.children[0].className = "dir-ico-opened";
						e.parentNode.children[1].className = "";
						var args = [];
						args.push(e);
						args.push(fileManager);
						args.push(ucleTabs);
						args.push(1);
						args.push(e.title);
						args.push("dir");
						promptInputAndAdd(args);
					},
					label: "Add a new folder"
				},
				{
					type: 'separator'
				},
				{
					click() {
						e.children[0].className = "dir-ico-opened";
						e.parentNode.children[1].className = "";
						var args = [];
						args.push(e);
						args.push(fileManager);
						args.push(ucleTabs);
						args.push(1);
						args.push(e.title);
						args.push("file");
						promptInputAndAdd(args);
					},
					label: "Add a new file"
				},
			];
		},
	contextWorkspaceDir:
		(fileManager, ucleTabs, e) => {
			return [
				{
					click() {
						var index = Array.from(e.parentNode.children).indexOf(e);
						e.children[0].className = "dir-ico-opened";
						e.parentNode.children[index + 1].className = "";
						var args = [];
						args.push(e.parentNode.children[index + 1]);
						args.push(fileManager);
						args.push(ucleTabs);
						args.push(index+1);
						args.push(e.title);
						args.push("dir");
						promptInputAndAdd(args);
					},
					label: "Add a new folder"
				},
				{
					type: 'separator'
				},				
				{
					click() {
						var index = Array.from(e.parentNode.children).indexOf(e);
						e.children[0].className = "dir-ico-opened";
						e.parentNode.children[index + 1].className = "";
						var args = [];
						args.push(e.parentNode.children[index + 1]);
						args.push(fileManager);
						args.push(ucleTabs);
						args.push(index+1);
						args.push(e.title);
						args.push("file");
						promptInputAndAdd(args);
					},
					label: "Add a new file"
				},
				{
					type: 'separator'
				},
				{
					label: "Rename",
					click() {
						promptInputAndRename(e, e.children[1], "dir", ucleTabs, fileManager);
					}
				},
				{
					label: "Delete folder",
					click() {
						deleteFile(e, ucleTabs, fileManager, "dir");
					}
				}				
			];
		},
	contextWorkspaceFiles:
		(fileManager, ucleTabs, e) => {

			return [
				{
					label: "Rename",
					click() {
						promptInputAndRename(e, e.children[1], "file", ucleTabs, fileManager);
					}
				},
				{
					label: "Delete",
					click() {
						deleteFile(e, ucleTabs, fileManager, "file");
					}
				}
			];
		},
	contextOpenedFiles:
		(fileManager, ucleTabs, e) => {
			return [
				{
					click() {
						ucleTabs.closeTabByPath(e.title);
					},
					label: "Close opened file"
				}
			];
		},
	contextListedFiles:
		(fileManager) => {
			return [
				{
					label: 'Add folder to workspace',
					click () {
						fileManager.openDirectory();
					}
				}
			];
		}
}