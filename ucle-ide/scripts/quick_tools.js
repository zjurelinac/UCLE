const { remote, ipcRenderer } = require('electron');

const Menu = remote.Menu;
const MenuItem = remote.MenuItem;
var simRunning = false;
var line = 1;
var address = 0;

const minFontSize = 8;
const maxFontSize = 32;

var themeRegChangeColor = "rgb(189, 189, 189)";
var themeRegColor = "rgb(225, 225, 225)";
var clickedElementColor = "#e6e6e6";

var haltReached = false;
var registersFlags = [
	{
		name: "IIF",
		title: "Interrupt flag"
	},
	{
		name: "PC",
		title: "Program Counter"
	},
	{
		name: "R0",
		title: ""
	},
	{
		name: "R1",
		title: ""
	},
	{
		name: "R2",
		title: ""
	},
	{
		name: "R3",
		title: ""
	},
	{
		name: "R4",
		title: ""
	},
	{
		name: "R5",
		title: ""
	},
	{
		name: "R6",
		title: ""
	},
	{
		name: "R7/SP",
		title: "Stack Pointer"
	},
	{
		name: "SR",
		title: "Status Register"
	},
	{
		name: "GIE",
		title: "Global Interrupt Enable"
	},
	{
		name: "Z",
		title: "Zero flag"
	},
	{
		name: "V",
		title: "Overflow flag"
	},
	{
		name: "C",
		title: "Carry flag"
	},
	{
		name: "N",
		title: "Negative flag"
	}
];

var clickedElement = null;

module.exports = (editor, fileManager, ucleTabs, ucleServer) => {

	function addButtonClick(button) {
		button.addEventListener("click", function(e) {
			if(fileManager.openDirectory()) {
				document.getElementById("quick-tools").style.whiteSpace = "nowrap";
			}
		});
	}

	function calculateWidth(running) {
		var tools = document.getElementById("quick-tools");
		var style = window.getComputedStyle(tools , null);

		if(style.display == "none") {
			if(running) {
				document.getElementById("show-sim").className = "show";			
				document.getElementById("show-sim").style.width = "19%";
				document.getElementById("container").style.width = "79%";
			} else {
				document.getElementById("show-sim").className = "hide";			
				document.getElementById("container").style.width = "100%";
			}
		} else {
			if(running) {
				document.getElementById("show-sim").className = "show";			
				document.getElementById("show-sim").style.width = "20%";
				document.getElementById("container").style.width = "60%";
			} else {
				document.getElementById("show-sim").className = "hide";			
				document.getElementById("container").style.width = "80%";
			}
		}
	}

	// ---------------------------------------------------------------------------------
	// ---------------------- Handle simulation functions ------------------------------
	// ---------------------------------------------------------------------------------

	function addSimEvents() {
		document.getElementById("step").className = "run-simulation";
		document.getElementById("continue").className = "run-simulation";
		document.getElementById("rm-breakpoints").className = "run-simulation";
		document.getElementById("reset").className = "run-simulation";

		document.getElementById("step").addEventListener("click", stepSim);
		document.getElementById("continue").addEventListener("click", continueSim);
		document.getElementById("rm-breakpoints").addEventListener("click", removeBreakPoints);
		document.getElementById("reset").addEventListener("click", resetProcessor);
	}

	function removeSimEvents() {
		document.getElementById("step").className = "wait-simulation";
		document.getElementById("continue").className = "wait-simulation";
		document.getElementById("rm-breakpoints").className = "wait-simulation";
		document.getElementById("reset").className = "wait-simulation";

		document.getElementById("step").removeEventListener("click", stepSim);
		document.getElementById("continue").removeEventListener("click", continueSim);
		document.getElementById("rm-breakpoints").removeEventListener("click", removeBreakPoints);
		document.getElementById("reset").addEventListener("click", resetProcessor);		
	}

	function clearRegisterInfo() {
		var registerTable = document.getElementById("registers").rows;
		
		for(var i = 1; i <= registersFlags.length; i++) {
			var row = registerTable[i];
			var regName = row.cells[0];
			var regValue = row.cells[1];
			if(regName.innerHTML == "IIF") {
				regValue.innerHTML = "true";
			} else {
				regValue.innerHTML = "0";
			}
			regValue.style.backgroundColor = "initial";
		}
	}

	function stepSim() {
		ucleServer.sendCommand("step",[]);
		setTimeout(function() {
			var model = ucleTabs.currentTabModel;
			line = ucleServer.findFirstNonEmpty(model, line, address);
			if(line == -1) {
				ucleServer.removeHighLight(model);
				document.getElementById("step").className = "wait-simulation";
				document.getElementById("step").removeEventListener("click", stepSim);
				document.getElementById("continue").className = "wait-simulation";
				document.getElementById("continue").removeEventListener("click", continueSim);
				return;
			}
			ucleServer.addHighLight(model, new monaco.Range(line,1,line,1));
			editor.revealLine(line);
			if(haltReached) {
				line = -1;
			}
		}, 100);
	}

	function continueSim() {
		ucleServer.sendCommand("cont",[]);
		setTimeout(function() {
			var model = ucleTabs.currentTabModel;
			line = ucleServer.findFirstBreakPoint(model, line, address);
			if(line == -1) {
				ucleServer.removeHighLight(model);

				document.getElementById("step").className = "wait-simulation";
				document.getElementById("step").removeEventListener("click", stepSim);
				document.getElementById("continue").className = "wait-simulation";
				document.getElementById("continue").removeEventListener("click", continueSim);
				return;
			} 
			ucleServer.addHighLight(model, new monaco.Range(line,1,line,1));
			editor.revealLine(line);

			if(haltReached) {
				line = -1;
			}
		}, 100);
	}

	function removeBreakPoints() {
		var model = ucleTabs.currentTabModel;
		ucleServer.removeBreakPoints(model);
	}

	function resetProcessor() {
		var model = ucleTabs.currentTabModel;
		ucleServer.sendCommand("reset",[]);
		line = 1;
		address = 0;
		editor.revealLine(line);

		removeSimEvents();
		addSimEvents();
		ucleServer.sendCommand("start",[]);
		ucleServer.resetPosition(model);
		haltReached = false;
	}

	// ---------------------------------------------------------------------------------
	// --------------------------- Handle font size  -----------------------------------
	// ---------------------------------------------------------------------------------

	document.getElementById("increase").addEventListener("click", function(e) {
		var value = document.getElementById("font-size").value;
		if(value == maxFontSize) return;
		document.getElementById("font-size").value = parseInt(value) + 2;
		editor.updateOptions({fontSize: document.getElementById("font-size").value});
	});

	document.getElementById("decrease").addEventListener("click", function(e) {
		var value = document.getElementById("font-size").value;
		if(value == minFontSize) return;
		document.getElementById("font-size").value = parseInt(value) - 2;
		editor.updateOptions({fontSize: document.getElementById("font-size").value});
	});

	document.getElementById("font-size").addEventListener("change", function(e) {
		var value = document.getElementById("font-size").value;
		if((value % 2)) {
			value++;
		}

		if(value >= maxFontSize) {
			value = maxFontSize;
		} else if(value <= minFontSize) {
			value = minFontSize;
		}

		document.getElementById("font-size").value = value;
		editor.updateOptions({fontSize: value});
	});

	// ---------------------------------------------------------------------------------
	// ------------------ Handle showing the files and sim divs ------------------------
	// ---------------------------------------------------------------------------------

	var listedFiles = document.getElementById("listed-files");
	var openedFiles = document.getElementById("opened-files");

	document.getElementById("show-sim").className = "hide";

	addButtonClick(document.getElementById("openbtn"));

	ipcRenderer.on('open-dir', (e) => {
		fileManager.openDirectory();
	});

	document.getElementById("refresh").addEventListener("click", function(e) {
		fileManager.reloadListedFiles();
	});

	listedFiles.addEventListener("click", function(e) {
		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir"))) {
			var child = e.target.children[0];
			if(child.className == "dir-ico-opened") {
				child.className = "dir-ico-closed";
			} else {
				child.className = "dir-ico-opened";
			}
			fileManager.readFolder(e.target.title, false, e.target);
		} else if((e.target.matches("i") || e.target.matches("span")) && 
			      (e.target.parentNode.matches("li.dir") || e.target.parentNode.matches("div.dir"))) {
			var child = e.target.parentNode.children[0];
			if(child.className == "dir-ico-opened") {
				child.className = "dir-ico-closed";
			} else {
				child.className = "dir-ico-opened";
			}
			fileManager.readFolder(e.target.parentNode.title, false, e.target.parentNode);
		}

		if (e.target && (e.target.matches("li.dir") || e.target.matches("div.dir") 
			|| e.target.matches("li.file"))) {
			var clicked = document.querySelector('[clicked-attr="click"]');
			if(clicked) {
				clicked.style.backgroundColor = "initial";
				clicked.setAttribute("clicked-attr","noclick");
			}
			e.target.style.backgroundColor = clickedElementColor;
			e.target.setAttribute("clicked-attr","click");
		} else if((e.target.matches("i") || e.target.matches("span")) && 
			(e.target.parentNode.matches("li.dir") || e.target.parentNode.matches("div.dir") 
			|| e.target.parentNode.matches("li.file"))) {
			var clicked = document.querySelector('[clicked-attr="click"]');
			if(clicked) {
				clicked.style.backgroundColor = "initial";
				clicked.setAttribute("clicked-attr","noclick");
			}
			e.target.parentNode.style.backgroundColor = clickedElementColor;
			e.target.parentNode.setAttribute("clicked-attr","click");
		}
	});

	listedFiles.addEventListener("contextmenu", function(e) {
		var template = null;
		if(simRunning) {
			var type = "warning";
			var buttons = ['OK'];
			var message = "Simulation is running! Please stop the simulation and try again!";
			var defaultId = 0;
			remote.dialog.showMessageBox({message, type, buttons, defaultId});
			return;
		} else if(e.target.matches("div.dir")) {
			template = require('./menus').contextWorkspace(fileManager, ucleTabs, e.target);
		} else if((e.target.matches("i") && e.target.parentNode.matches("div.dir"))) {
			template = require('./menus').contextWorkspace(fileManager, ucleTabs, e.target.parentNode);			
		} else if(e.target.matches("li.dir")) {
			template = require('./menus').contextWorkspaceDir(fileManager, ucleTabs, e.target);
		} else if((e.target.matches("i") ||
			       e.target.matches("span")) && 
			       e.target.parentNode.matches("li.dir")) {
			template = require('./menus').contextWorkspaceDir(fileManager, ucleTabs, e.target.parentNode);
		} else if(e.target.matches("li.file")) {
			template = require('./menus').contextWorkspaceFiles(fileManager, ucleTabs, e.target);
		} else if((e.target.matches("i") ||
			       e.target.matches("span")) && 
			       e.target.parentNode.matches("li.file")) {
			template = require('./menus').contextWorkspaceFiles(fileManager, ucleTabs, e.target.parentNode);			
		} else {
			template = require('./menus').contextListedFiles(fileManager);
		}
		const menu = Menu.buildFromTemplate(template);
		menu.popup(remote.getCurrentWindow());
	});

	listedFiles.addEventListener("dblclick", function(e) {
		if(simRunning) {
			var type = "warning";
			var buttons = ['OK'];
			var message = "Simulation is running! Please stop the simulation and try again!";
			var defaultId = 0;
			remote.dialog.showMessageBox({message, type, buttons, defaultId});
			return;
		} else if(e.target && e.target.matches("li.file")) {
			fileManager.openFile(e.target.title);
			ucleTabs.addTab({title: ucleTabs.getFileName(e.target.title), fullPath: e.target.title},true);	
		} else if((e.target.matches("i") || e.target.matches("span")) && e.target.parentNode.matches("li.file")) {
			fileManager.openFile(e.target.parentNode.title);
			ucleTabs.addTab({title: ucleTabs.getFileName(e.target.parentNode.title), fullPath: e.target.parentNode.title},true);			
		}
	});

	openedFiles.addEventListener("dblclick", function(e) {
		if(simRunning) {
			var type = "warning";
			var buttons = ['OK'];
			var message = "Simulation is running! Please stop the simulation and try again!";
			var defaultId = 0;
			remote.dialog.showMessageBox({message, type, buttons, defaultId});
			return;
		} else if ((e.target.matches("div.file") || e.target.matches("span"))) {
			ucleTabs.setCurrentTabByPath(e.target.title);
		}
	});

	openedFiles.addEventListener("contextmenu", function(e) {
		if(simRunning) {
			var type = "warning";
			var buttons = ['OK'];
			var message = "Simulation is running! Please stop the simulation and try again!";
			var defaultId = 0;
			remote.dialog.showMessageBox({message, type, buttons, defaultId});
			return;
		}
		template = require('./menus').contextOpenedFiles(fileManager, ucleTabs, e);
		const menu = Menu.buildFromTemplate(template);
		menu.popup(remote.getCurrentWindow());
	});

	document.getElementById("file-explorer").addEventListener("click", function(e) {
		var tools = document.getElementById("quick-tools");
		var style = window.getComputedStyle(tools , null);
		var sim = document.getElementById("show-sim");
		var lineCount = document.getElementById("line-column-count");

		editor.focus();

		if(style.display == "none") {
			tools.style.display = "inline-block";
			tools.style.width = "18%";
			lineCount.style.left = "19%";
		} else {
			tools.style.display = "none";
			tools.style.width = "0%";
			lineCount.style.left = "1%";
		}

		calculateWidth(simRunning);
	});

	document.getElementById("workspace").addEventListener("click", function(e) {
		var workspace = document.getElementById('workspace');

		if(workspace.children[0].className == "explorer-ico-opened") {
			workspace.children[0].className = "explorer-ico-closed";
			listedFiles.className = "hide";
		} else {
			workspace.children[0].className = "explorer-ico-opened";
			listedFiles.className = "";
		}
	});

	document.getElementById("explorer").addEventListener("click", function(e) {
		if(!document.querySelector('[id^="open-"')) {
			return;
		}

		var explorer = document.getElementById('explorer');

		if(explorer.children[0].className == "explorer-ico-opened") {
			explorer.children[0].className = "explorer-ico-closed";
			openedFiles.className = "hide";
		} else {
			explorer.children[0].className = "explorer-ico-opened";
			openedFiles.className = "";
		}
	});

	// ---------------------------------------------------------------------------------
	// --------------- Init the register state and change flag value--------------------
	// ---------------------------------------------------------------------------------


	function addFlagValue(registerJSON) {
		var srRegVal;
		var length = registerJSON.length;

		for(var i = 0; i < length; i++) {
			if(registerJSON[i][0] == "R7") {
				registerJSON.splice(i,1);
			} else if(registerJSON[i][0] == "SR") {
				srRegVal = registerJSON[i][1];
				break;
			}
		}
		var binary = ("00000000" + srRegVal.toString(2)).substr(-8);

		for (var i = 3; i < binary.length; i++) {
			var flagName = registersFlags[registersFlags.length-binary.length+i].name;
			registerJSON.push([flagName, binary.charAt(i)]);
		}
	}

	function initRegs() {
		var regTable = document.getElementById("registers");

		var i = 1;

		for(; i <= registersFlags.length; i++) {
			var row = regTable.insertRow(i);

			var regName = row.insertCell(0);
			var regValue = row.insertCell(1);

			var regFlag = registersFlags[i-1];
			regName.innerHTML = regFlag.name;
			if(regFlag.title != "") {
				regName.setAttribute("title", regFlag.title);
			}

			if(regFlag.name == "IIF") {
				regValue.innerHTML = "true";
			} else {
				regValue.innerHTML = "0";
			}
		}
	}

	// ---------------------------------------------------------------------------------
	// ------------- Handle the simulation start and response --------------------------
	// ---------------------------------------------------------------------------------

	initRegs();

	function stopSimulation(target) {
		target.className = "run-simulation";
		removeSimEvents();
		calculateWidth(false);

		simRunning = false;
		ucleTabs.simRunning= false;
		fileManager.simRunning = false;
		haltReached = false;

		line = 1;
		address = 0;
		editor.revealLine(line);

		editor.updateOptions({readOnly: false, selectionHighlight: true});
		clearRegisterInfo();
		ucleServer.removeHighLight(ucleTabs.currentTabModel);

		ucleTabs.showAllTabs();
	}

	function startSimulation(filePath, target) {
		var error = ucleServer.getMachineCodeAndRun(filePath, ucleTabs.currentTabModel);

		if(error) {
			var type = "warning";
			var buttons = ['OK'];
			var message = 'Error in getting the machine code!\nPlease check your code for errors!';
			var defaultId = 0;
			remote.dialog.showMessageBox({message, type, buttons, defaultId});
			stopSimulation(target);
		}
	}
	document.getElementById("run-sim").addEventListener("click", function(e) {
		if(e.target.className == "run-simulation") {
			var currTabValue = ucleTabs.currentTabValue;
			if(currTabValue) {
				var filePath = ucleTabs.currentTab.querySelector('.ucle-tab-file-path').textContent;
				if(ucleTabs.getExtension(filePath) != "s"){
					var type = "warning";
					var buttons = ['OK'];
					var message = "Cannot run simulation on file without '.s' extension!";
					var defaultId = 0;
					remote.dialog.showMessageBox({message, type, buttons, defaultId});
					return;
				}
				ucleTabs.hideTabs();
				e.target.className = "stop-simulation";

				addSimEvents();
				calculateWidth(true);

				simRunning = true;
				ucleTabs.simRunning= true;
				fileManager.simRunning = true;
				editor.updateOptions({readOnly: true, selectionHighlight: false});

				var changed = ucleTabs.currentTab.querySelector(".ucle-tab-content-changed");
				if(changed) {
					fileManager.saveFile(filePath);
					ucleTabs.updateTabContent(ucleTabs.currentTab);
				}

				startSimulation(filePath, e.target);
			} else {
				var type = "warning";
				var buttons = ['OK'];
				var message = 'Cannot run simulation with an empty file!';
				var defaultId = 0;
				remote.dialog.showMessageBox({message, type, buttons, defaultId});
			}
		} else {
			stopSimulation(e.target);
			ucleServer.stopSim(ucleTabs.currentTabModel);
		}	
	});

	ipcRenderer.on('run-simulation', (e) => {
		var sim = document.getElementById("run-sim");
		if(sim.className == "run-simulation") {
			var currTabValue = ucleTabs.currentTabValue;
			if(currTabValue) {
				var filePath = ucleTabs.currentTab.querySelector('.ucle-tab-file-path').textContent;
				if(ucleTabs.getExtension(filePath) != "s"){
					var type = "warning";
					var buttons = ['OK'];
					var message = "Cannot run simulation on this file!";
					var defaultId = 0;
					var title = "Warning";
					remote.dialog.showMessageBox({message, type, buttons, defaultId, title});
					return;
				}
				ucleTabs.hideTabs();
				sim.className = "stop-simulation";

				addSimEvents();
				calculateWidth(true);

				simRunning = true;
				ucleTabs.simRunning= true;
				fileManager.simRunning = true;
				editor.updateOptions({readOnly: true, selectionHighlight: false});

				var changed = ucleTabs.currentTab.querySelector(".ucle-tab-content-changed");
				if(changed) {
					fileManager.saveFile(filePath);
					ucleTabs.updateTabContent(ucleTabs.currentTab);
				}

				startSimulation(filePath, e.target);
			} else {
				var type = "warning";
				var buttons = ['OK'];
				var message = 'Cannot run simulation with an empty file!';
				var defaultId = 0;
				var title = "Warning";
				remote.dialog.showMessageBox({message, type, buttons, defaultId, title});
			}
		} else {
			stopSimulation(sim);
			ucleServer.stopSim(ucleTabs.currentTabModel);
		}	
	});

	ipcRenderer.on('sim-response', (e, data) => {
		var dataArray = data.split("\n");

		dataArray.forEach(function(dataElem) {
			if(dataElem == "") return;
			var json = JSON.parse(dataElem);

			if(json.location) {
				address = json.location;
			}

			if(json.state == "stopped" && json.annotation.startsWith("HALT")) {
				haltReached = true;
				return;
			}

			if(json.type == "register_info") {
				var registerJSON = Object.keys(json.registers).map(function(key) {
					return [String(key), json.registers[key]];
				});
				addFlagValue(registerJSON);

				var registerTable = document.getElementById("registers").rows;
				
				for(var i = 1; i <= registersFlags.length; i++) {
					var row = registerTable[i];
					var regValue = row.cells[1];

					var readData = registerJSON[i-1][1].toString();
					if(regValue.innerHTML != readData) {
						regValue.style.backgroundColor = themeRegChangeColor;
						regValue.innerHTML = readData;

						setTimeout(function() {	
							for(var i = 1; i <= registersFlags.length; i++) {
								var row = registerTable[i];
								var regValue = row.cells[1];
								if(regValue.style.backgroundColor == themeRegChangeColor) {
									regValue.style.backgroundColor = themeRegColor;
									return;
								}
							}
						}, 400);
					}
				}
			}
		})
	});


	// ---------------------------------------------------------------------------------
	// ------------------------- Handle the theme change -------------------------------
	// ---------------------------------------------------------------------------------

	function changeRegColors(oldThemeRegChangeColor, oldThemeRegColor) {
		var registerTable = document.getElementById("registers").rows;
	
		for(var i = 1; i <= registersFlags.length; i++) {
			var row = registerTable[i];
			var regValue = row.cells[1];
			if(regValue.style.backgroundColor == oldThemeRegChangeColor) {
				regValue.style.backgroundColor = themeRegChangeColor;
			} else if(regValue.style.backgroundColor == oldThemeRegColor) {
				regValue.style.backgroundColor = themeRegColor;
			}
		}
	}

	ipcRenderer.on("light-theme-change", (e) => {
		var oldThemeRegChangeColor = themeRegChangeColor;
		var oldThemeRegColor = themeRegColor;		
		themeRegChangeColor = "rgb(189, 189, 189)";
		themeRegColor = "rgb(225, 225, 225)";
		clickedElementColor = "#e6e6e6";
		changeRegColors(oldThemeRegChangeColor, oldThemeRegColor);
	});

	ipcRenderer.on("dark-theme-change", (e) => {
		var oldThemeRegChangeColor = themeRegChangeColor;
		var oldThemeRegColor = themeRegColor;
		themeRegChangeColor = "rgb(100, 100, 100)";	
		themeRegColor = "rgb(60, 60, 60)";
		clickedElementColor = "#606060";
		changeRegColors(oldThemeRegChangeColor, oldThemeRegColor);
	});

};