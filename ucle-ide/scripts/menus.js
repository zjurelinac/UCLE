const dialog = require('electron').dialog;

module.exports = (mainWindow) => {
	
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
				label: 'Reload',
				accelerator: 'CmdOrCtrl+R',
				click (item, focusedWindow) {
					if (focusedWindow) focusedWindow.reload();
				}
			},
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
]
}