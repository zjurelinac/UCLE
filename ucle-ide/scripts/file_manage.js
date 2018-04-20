const remote = require('electron').remote;
const ipcRenderer = require('electron').ipcRenderer;
const fs = require('fs');
const path = require('path');
const mime = require('mime');

class FileManager {

	constructor({ editor, monaco }) {
		this.editor = editor;
		this.monaco = monaco;
		this.startPath = './';

		ipcRenderer.on('open-file', (e, file) => this.openFile(file));
		ipcRenderer.on('save-as-file', (e) => this.saveAsFile());
	}


	openFile(file) {
		let data = fs.readFileSync(file, 'utf-8');
		this.editor.setModel(this.monaco.editor.createModel(data));
	}

	openDirectory() {
		var filename = remote.dialog.showOpenDialog({ properties: ['openDirectory']});
		if(!filename) return;
		this.readFolder(filename.toString() + '/');
	}

	saveAsFile() {
		var filename = remote.dialog.showSaveDialog();

		if(!filename) return;

		const model = this.editor.getModel();

		fs.writeFile(filename.toString(), model.getValue(), 'utf-8', function(err) {
			if(err) {
				throw err;
			}
		});

		return filename;
	}

	saveFile(file) {
		const model = this.editor.getModel();
		fs.writeFile(file, model.getValue(), 'utf-8', function(err) {
			if(err) {
				throw err;
			}
		});
	}

	readFolder(filePath) {
		if(filePath === null || filePath === undefined) { 
			filePath = './'
		}

		this.startPath = filePath;

		fs.readdir(filePath, (err, files) => {
			'use strict';
			if (err) throw  err;
			document.getElementById('listed-files').innerHTML = `<ul id="display-files"></ol>`;
			for (let file of files) {
				fs.stat(filePath + file, (err, stats) => {
					let theID = `${filePath}${file}/`;
					if (err) throw err;
					if (stats.isDirectory()) {
						document.getElementById('display-files').innerHTML += `<li id=${theID} class="dir"><i class="far fa-folder"></i> ${file}</li> <br/>`;
					} else {
						theID = theID.slice(0, -1);
						document.getElementById('display-files').innerHTML += `<li id=${theID} class="file"><i class="far fa-file"></i>  ${file}</li> <br/>`;
					}

					document.getElementById(theID).style.padding = "5px 5px 0px 0px";
				});
			}
		});
	}
}

module.exports = FileManager;