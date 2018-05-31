const dialog = require('electron').dialog;
const remote = require('electron').remote;

module.exports = {
	contextMain:
		(mainWindow) => {
		
			const webContents = mainWindow.webContents;

			var openedFile;

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
								dialog.showOpenDialog({properties: ['openFile']}, function(filename) { 
									if(filename !== undefined) webContents.send('open-file', filename.toString());
									openedFile = filename;
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
								webContents.send('save-file', openedFile);
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
							click () { require('electron').shell.openExternal('http://electron.atom.io'); }
						},
						{
							label: 'About',
							click () { console.log('Link will be here.'); }
						}
					]
				}
			];
		},
	contextWorkspace:
		(fileManager, e) => {

			function removeFolder(folder) {
				console.log(folder);
				fileManager.removeFolder(folder.id); 
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
						removeFolder(e.target);
					},
					label: "Remove folder from workspace"
				},
				{
					click() {
						console.log("soadk");
					},
					label: "Add a new file"
				},
			];
		},
	contextWorkspaceFiles:
		(fileManager, ucleTabs, e) => {

			function promptInputAndRename(parent, child) {
				var input = document.createElement("input");
				input.value = child.innerHTML;
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
					var newPath = parent.id.replace(/[^\/]*$/, '') + newName;
					var oldPath = parent.id;
					child.innerHTML = newName;
					parent.id = newPath;
					parent.title = newPath;
					parent.replaceChild(child, input);
					fileManager.renameFile(oldPath, newPath);
					if(ucleTabs.checkIfTabOpened(oldPath)) {
						ucleTabs.updateTab(ucleTabs.getTabByPath(oldPath), {title: newName, fullPath: newPath});
						ucleTabs.updateTabContent(ucleTabs.getTabByPath(oldPath));
						var opened = document.getElementById("open-" + oldPath);
						opened.id = "open-" + newPath;
						opened.title = newPath;
						opened.children[1].innerHTML = newName;
					}
				});
			}

			function deleteFile(file) {
				var type = "question";
				var buttons = ['No','Yes'];
				var message = 'Are you sure you want to delete the file?\nUnsaved progress will be lost!';
				var defaultId = 0;
				var response = remote.dialog.showMessageBox({message, type, buttons, defaultId});

				if(response) {
					fileManager.deleteFile(file.id);
					if(ucleTabs.checkIfTabOpened(file.id)) {
						ucleTabs.removeTab(ucleTabs.getTabByPath(file.id));
					}
					file.parentNode.removeChild(file);
				}
			}


			return [
				{
					label: "Rename",
					click() {
						if(e.target.matches("li.file")) {
							promptInputAndRename(e.target, e.target.children[1], fileManager);
						} else if((e.target.matches("i") || e.target.matches("span")) && e.target.parentNode.matches("li.file")) {
							promptInputAndRename(e.target.parentNode, e.target.parentNode.children[1], ucleTabs);
						}
					}
				},
				{
					label: "Delete",
					click() {
						if(e.target.matches("li.file")) {
							deleteFile(e.target);
						} else if((e.target.matches("i") || e.target.matches("span")) && e.target.parentNode.matches("li.file")) {
							deleteFile(e.target.parentNode);
						}
					}
				}
			];
		},
	contextOpenedFiles:
		(fileManager, ucleTabs, e) => {
			return [
				{
					click() {
						ucleTabs.closeTabByPath(e.target.title);
					},
					label: "Close opened file"
				}
			];
		}
}