const { app, BrowserWindow, Menu } = require("electron")

let mainWindow, template

function createWindow () {
	mainWindow = new BrowserWindow({width: 800, height: 600})
	mainWindow.loadURL(`file://${__dirname}/index.html`)
	mainWindow.on('closed', function () {
		mainWindow = null
	})

	template = require('./scripts/menus')(mainWindow);

	const menu = Menu.buildFromTemplate(template)
	Menu.setApplicationMenu(menu)
}

app.on('ready', function () {
	createWindow()
})

app.on('window-all-closed', function () {
	if (process.platform !== 'darwin') {
		app.quit()
	}
})

app.on('activate', function () {
	if (mainWindow === null) {
		createWindow()
	}
})