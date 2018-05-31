const dialog = require('electron').dialog;

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
			return [
				{
					click() {
						fileManager.removeFolder(e.target.id); 
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
					},
					label: "Remove folder from workspace"
				},
				{
					click() {
						console.log("soadk");
					},
					label: "Add a new file"
				},
				{
					click() {
						if(e.target.matches("li.file")) {
							var input = document.createElement("input");
							input.value = e.target.children[1].innerHTML;
							console.log(e.target.children[1]);
							e.target.replaceChild(input, e.target.children[1]);
							input.focus();
						} else if((e.target.matches("i") || e.target.matches("span")) && e.target.parentNode.matches("li.file")) {
							e.target.parentNode.children[1].setAttribute("contenteditable","true");
						}
					},
					label: "Rename file"
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