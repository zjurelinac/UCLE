const { app, BrowserWindow, Menu } = require("electron");
const dialog = require('electron').dialog;
let mainWindow, template;

function createWindow () {
	mainWindow = new BrowserWindow({width: 800, height: 600});
	mainWindow.loadURL(`file://${__dirname}/index.html`);

	mainWindow.on('close', function(e){
		var type = "question";
		var buttons = ['No','Yes'];
		var message = 'Are you sure you want to quit?\n\nAll unsaved progress will be lost!';
		var defaultId = 0;
		title = "Question";
		var response = dialog.showMessageBox({message, type, buttons, defaultId, title});
		if(response == 0) {
			e.preventDefault();
		}
	});

	mainWindow.on('closed', function () {
		mainWindow = null;
	});

	template = require('./scripts/menus').contextMain(mainWindow);

	const menu = Menu.buildFromTemplate(template);
	Menu.setApplicationMenu(menu);
}

app.on('ready', function () {
	app.commandLine.appendSwitch('high-dpi-support', 'true');
	app.commandLine.appendSwitch('force-device-scale-factor', '1');
	createWindow();
});

app.on('window-all-closed', function () {
	if (process.platform !== 'darwin') {
		app.quit();
	}
});

app.on('activate', function () {
	if (mainWindow === null) {
		createWindow();
	}
});