const { remote, ipcRenderer } = require('electron');

module.exports = (fileManager, ucleTabs) => {

	document.getElementById("listed-files").addEventListener("click", function(e) {
		if (e.target && (e.target.matches("li.dir") || e.target.matches("span"))) {
			var child = e.target.children[0]; 
			if(child.className == "dir-ico-opened") {
				child.className = "dir-ico-closed";
			} else {
				child.className = "dir-ico-opened";
			}
			fileManager.readFolder(e.target.id, false);
		}
	});

	document.getElementById("listed-files").addEventListener("dblclick", function(e) {
		if(e.target && e.target.matches("li.file")) {
			fileManager.openFile(e.target.id);
			ucleTabs.addTab({title: ucleTabs.getFileName(e.target.id), fullPath: e.target.id},true);
		}
	});

	document.getElementById("listed-files").addEventListener("mouseover", function(e) {
		e.preventDefault();
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file") || e.target.matches("span"))) {
			document.getElementById(e.target.id).style.cursor = "pointer";
			document.getElementById(e.target.id).style.background = "#e9e9e9"
		}
	});

	document.getElementById("listed-files").addEventListener("mouseout", function(e) {
		e.preventDefault();
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file") || e.target.matches("span"))) {
			document.getElementById(e.target.id).style.background = "white";
		}
	});

	document.getElementById("explorer").addEventListener("click", function(e) {
		var tools = document.getElementById("quick-tools");
		var style = window.getComputedStyle(tools , null);

		if(style.display == "none") {
			tools.style.display = "inline-block";
			document.getElementById("container").style.width = "78%";
			tools.style.width = "20%";
		} else {
			tools.style.display = "none";
			document.getElementById("container").style.width = "100%";
			tools.style.width = "0%";
		}
	});

	document.getElementById("openbtn").addEventListener("click", function(e) {
		fileManager.openDirectory();
		ucleTabs.closeAllTabs();
	});

	ipcRenderer.on('open-dir', (e) => {
		fileManager.openDirectory();
	});
};