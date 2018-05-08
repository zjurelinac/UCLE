const { remote, ipcRenderer } = require('electron');

const Menu = remote.Menu;
const MenuItem = remote.MenuItem;

module.exports = (editor, fileManager, ucleTabs, ucleServer) => {

	document.getElementById("listed-files").addEventListener("click", function(e) {
		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir"))) {
			var child = e.target.children[0]; 
			if(child.className == "dir-ico-opened") {
				child.className = "dir-ico-closed";
			} else {
				child.className = "dir-ico-opened";
			}
			fileManager.readFolder(e.target.id, false);
		}
	});

	function addButtonClick(button) {
		button.addEventListener("click", function(e) {
			fileManager.openDirectory();
		});
	}

	document.getElementById("listed-files").addEventListener("contextmenu", function(e) {
		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir"))) {
			var contextMenu = new Menu();

			contextMenu.append(new MenuItem({
				click() {
					fileManager.removeFolder(e.target.id); 
					if(!document.querySelector('[id^="div-"')) {
						document.getElementById('workspace-text').style.display = "inline-block";
						var button = document.getElementById("openbtn");
						button.style.display = "block";
						addButtonClick(button);
					}
				},
				label: "Remove folder from workspace"
			}));

			contextMenu.popup(remote.getCurrentWindow());
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
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file") || e.target.matches("div.dir"))) {
			document.getElementById(e.target.id).style.cursor = "pointer";
			document.getElementById(e.target.id).style.background = "#e9e9e9"
		}
	});

	document.getElementById("listed-files").addEventListener("mouseout", function(e) {
		e.preventDefault();
		if(e.target && (e.target.matches("li.dir") || e.target.matches("li.file") || e.target.matches("div.dir"))) {
			document.getElementById(e.target.id).style.background = "white";
		}
	});

	document.getElementById("file-explorer").addEventListener("click", function(e) {
		var tools = document.getElementById("quick-tools");
		var style = window.getComputedStyle(tools , null);
		editor.focus();

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

	document.getElementById("run-sim").addEventListener("click", function(e) {
		if(e.target.className == "run-simulation") {
			var currTabValue = ucleTabs.currentTabValue;
			if(currTabValue) {
				ucleTabs.hideTabs();
				e.target.className = "stop-simulation";
				ucleServer.registerBreakPoints(ucleTabs.currentTabModel);
				ucleServer.runSim("1.p");
			} else {
				var type = "warning";
				var buttons = ['OK'];
				var message = 'Cannot run simulation with an empty file!';
				var defaultId = 0;
				var response = remote.dialog.showMessageBox({message, type, buttons, defaultId});
			}
		} else {
			e.target.className = "run-simulation";
			ucleServer.stopSim(ucleTabs.currentTabModel);
		}	
	});

	ipcRenderer.on('open-dir', (e) => {
		fileManager.openDirectory();
	});

	document.getElementById("workspace").addEventListener("click", function(e) {
		var workspace = document.getElementById('workspace');
		var listedFiles = document.getElementById('listed-files');
		var style = window.getComputedStyle(listedFiles , null);

		if(workspace.children[0].className == "explorer-ico-opened") {
			workspace.children[0].className = "explorer-ico-closed";
			listedFiles.style.display = "none";
		} else {
			workspace.children[0].className = "explorer-ico-opened";
			listedFiles.style.display = "inherit";			
		}
	});

	document.getElementById("explorer").addEventListener("click", function(e) {
		var explorer = document.getElementById('explorer');

		if(explorer.children[0].className == "explorer-ico-opened") {
			explorer.children[0].className = "explorer-ico-closed";
		} else {
			explorer.children[0].className = "explorer-ico-opened";
		}
	});

	ipcRenderer.on('run-simulation', (e) => {
		var sim = document.getElementById("run-sim");
		if(sim.className == "run-simulation") {
			var currTabValue = ucleTabs.currentTabValue;
			if(currTabValue) {
				ucleTabs.hideTabs();
				sim.className = "stop-simulation";
				ucleServer.registerBreakPoints(ucleTabs.currentTabModel);
				ucleServer.runSim("1.p");
			} else {
				var type = "warning";
				var buttons = ['OK'];
				var message = 'Cannot run simulation with an empty file!';
				var defaultId = 0;
				var response = remote.dialog.showMessageBox({message, type, buttons, defaultId});
			}
		} else {
			sim.className = "run-simulation";
			ucleServer.stopSim(ucleTabs.currentTabModel);
		}	
	});

	ipcRenderer.on('sim-response', (e, data) => {
		console.log(data);
		document.getElementById("run-sim").className = "run-simulation";
		ucleTabs.showAllTabs();
	});

	addButtonClick(document.getElementById("openbtn"));
};