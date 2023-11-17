var profilesMap = new Map();

class switchConfig{
    constructor(){
        this.macroBackgroundColor = 0x0000; //Black or nothing in a LED
        this.nameMacro = '';
    }

    setMacroName(name){
        this.nameMacro = name.trim().toString();
    }

    getMacroName(){
        return this.nameMacro;
    }

    setBackgroundColor(colorCode){this.macroBackgroundColor = colorCode;}

    getBackgroundColor(){return this.macroBackgroundColor;}

    setMacroList(macroList){
    }

    getMacroList(){
    }
}

class macroProfile{
    constructor(name){
        this.profileName = name;
        this.switchMap = new Map();
        this.macroBackgroundColor = 0x0000; //Black or nothing in a LED
    }

    setProfileName(name){this.profileName = name;}

    getProfileName(){return this.profileName;}

    setBackgroundColor(colorCode){this.macroBackgroundColor = colorCode;}

    getBackgroundColor(){return this.macroBackgroundColor;}

    addSwitchConfig(switchCode, switchConfig){
        if(this.switchMap.has(switchCode) == false)
        {
            this.switchMap.set(switchCode, switchConfig);
        }
    }

    getSwitchConfig(switchCode){
        if(this.switchMap.has(switchCode) == false)
        {
            var switchCFG = this.switchMap.get(parseInt(switchCode, 10));
            return switchCFG;
        }
    }
}


function updateMacroSwitchMatrix(profileName){
    //Set visibile all screen elements
    var screen = document.getElementById('screen');
    screen.style.visibility = 'visible';

    var profileConfig = profilesMap.get(profileName);
    var macroSwitch = document.querySelectorAll('.macroSwitch');

    macroSwitch.forEach(function(actualSwitch) {
        var profileConfigData = profileConfig.getSwitchConfig(actualSwitch.id);
        actualSwitch.textContent = profileConfigData.getMacroName();

        actualSwitch.onclick = function(){
            console.log(profileConfigData.getMacroName());
        }
    });
  
}

function setProfileTitle(profileName){
    var profileTitle = document.getElementById('profileTitle');
    profileTitle.textContent = profileName;
}

function newProfileTag(){

    let profileTagInput = document.getElementById('profileTagInput');
    var profileTagList = document.getElementById('profileTagList');

    //Check if we already save this profile
    if(profilesMap.has(profileTagInput.value) == false)
    {
        //Add to HTML new profile entry
        var newProfile = document.createElement('div');
        newProfile.innerHTML = 
        `<label id="${profileTagInput.value}Label">${profileTagInput.value}</label>
        <button id="${profileTagInput.value}DeleteButton"> - </button>`;

        profileTagList.appendChild(newProfile);

        //Get profile buttons to interact
        var profileLabel = document.getElementById(profileTagInput.value + "Label");
        var profileDelete = document.getElementById(profileTagInput.value + "DeleteButton");

        //Instance a new profile and save into the map with profile name as key
        const profileInstance = new macroProfile(profileTagInput.value);
        profilesMap.set(profileTagInput.value, profileInstance);

        console.log("Creating profile: " + profileTagInput.value);

        //Set initial switch config
        for(i = 0; i < 12; i++)
        {
            const switchConfigInstance = new switchConfig();
            switchConfigInstance.setMacroName(profileTagInput.value + i);
            profileInstance.addSwitchConfig(i, switchConfigInstance);
        }
        
        //Set callbacks
        profileLabel.onclick = function(){
            setProfileTitle(profileLabel.textContent)
            updateMacroSwitchMatrix(profileLabel.textContent);
        }

        profileDelete.onclick = function(){

        }

        setProfileTitle(profileTagInput.value)
        updateMacroSwitchMatrix(profileTagInput.value);
    }
}

