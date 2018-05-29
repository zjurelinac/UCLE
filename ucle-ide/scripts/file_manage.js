const remote = require('electron').remote;
const ipcRenderer = require('electron').ipcRenderer;
const fs = require('fs');
const path = require('path');
const mime = require('mime');
const dirTree = require('directory-tree');

class FileManager {

	constructor({ editor, monaco }) {
		this.editor = editor;
		this.monaco = monaco;
		this.startPath = './';
		this.simRunning = false;

		ipcRenderer.on('open-file', (e, file) => this.openFile(file));
		ipcRenderer.on('save-as-file', (e) => this.saveAsFile());
	}

	getFileName(str) {
		return str.split('\\').pop().split('/').pop();
	}

	getDirName(str) {
		return str.match(/([^\/]*)\/*$/)[1];
	}

	openFile(file) {
		if(this.simRunning) return;
		let data = fs.readFileSync(file, 'utf-8');
		this.editor.setModel(this.monaco.editor.createModel(data));
	}

	openDirectory() {
		var filename = remote.dialog.showOpenDialog({ properties: ['openDirectory']});
		if(!filename || document.getElementById("div-" + filename.toString())) return false;
		this.readFolder(filename.toString());
		return true;
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

	walkThroughFiles(children, listID) {
		let list = document.getElementById(listID);

		for (var i = 0, len = children.length; i < len; i++) {
			var child = children[i];
			let childListID = child.path + '-' + child.name;

			if(child.type == 'directory') {
				var dir = document.createElement("li");
				dir.id = child.path;
				dir.title = child.path;
				dir.className = "dir";

				var ico = document.createElement("i");
				ico.className = "dir-ico-closed";

				var dirName = document.createTextNode(child.name);

				var fileList = document.createElement("ul");
				fileList.id = childListID;

				dir.appendChild(ico);
				dir.appendChild(dirName);

				list.appendChild(dir);
				list.appendChild(fileList);
			} else {
				var file = document.createElement("li");
				file.id = child.path;
				file.title = child.path;
				file.className = "file";

				var ico = document.createElement("i");
				ico.className = "file-ico";

				var fileName = document.createTextNode(child.name);

				file.appendChild(ico);
				file.appendChild(fileName);
				list.appendChild(file);
				list.appendChild(document.createElement("br"));
			}
		}

		let elements = list.getElementsByTagName("li");

		var parentNode = list.parentNode.firstChild;

		if(parentNode && parentNode.firstChild.style) {
			for(var i = 0; i < elements.length; i++) {
				elements[i].getElementsByTagName("i")[0].style.marginLeft = parseInt(parentNode.firstChild.style.marginLeft, 10) + 15 + "px";
			}
		}
	}

	readFolder(filePath, init = true) {
		if(filePath === null || filePath === undefined) { 
			if(process.platform == "win32") {
				filePath: "C:\\";
			} else {
				filePath: "/";
			}
		}

		var dirName = this.getDirName(filePath);

		let listID = filePath + '-' + dirName;

		var list = document.getElementById(listID);

		if(list != null && list.innerHTML != '') {
			if(list.style.display == 'none') {
				list.style.display = 'block';
			} else {
				list.style.display = 'none';
			}
			return;
		}
		const tree = dirTree(filePath);

		if(init) {
			document.getElementById('openbtn').style.display = "none";
			document.getElementById('workspace-text').style.display = "none";
			document.getElementById('workspace').children[0].className = "explorer-ico-opened";
			//var folderWorkspace = document.createDocumentFragment();
			document.getElementById('listed-files').innerHTML += '<div id="div-' + filePath + '" style="width:100%"><div id="'
                                                              + filePath + '" title="' + filePath + '" style:"width:100%; display:block;" ' +
                                                              'class="dir"><i class="dir-ico-closed" style="margin-left:10px"></i>' + dirName + '</div><ul id="' + listID + 
                                                              '"style="display: none; width:100%"></ul></div>';
		}

		this.walkThroughFiles(tree.children, listID);
	}

	removeFolder(filePath) {
		var folder = document.getElementById("div-" + filePath);
		folder.parentNode.removeChild(folder);
	}
}

module.exports = FileManager;