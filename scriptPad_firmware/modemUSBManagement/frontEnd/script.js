var profileInputBox = document.getElementById("profileInputBox");
var switchMacroTextInputBox = document.getElementById("switchMacroTextInputBox");
var switchKeyMacroSelect = document.getElementById("switchKeyMacroSelect");
var matrixBCKEffectsDropdown = document.getElementById("matrixBCKEffectsDropdown");
const matrixButton = document.getElementsByClassName("matrixNewNameInputBox");
const profileList = document.getElementById("profileList");
const macroList = document.getElementById("macroList");
var colorPrim = document.getElementById("colorPrim");
var colorSec = document.getElementById("colorSec");

var lastProfile = null;
var selectedSwitchID = null;
var profilesMap = new Map();

const keyDefines = {
    KEY_NONE: 0x00,
    KEY_A: 0x04,
    KEY_B: 0x05,
    KEY_C: 0x06,
    KEY_D: 0x07,
    KEY_E: 0x08,
    KEY_F: 0x09,
    KEY_G: 0x0A,
    KEY_H: 0x0B,
    KEY_I: 0x0C,
    KEY_J: 0x0D,
    KEY_K: 0x0E,
    KEY_L: 0x0F,
    KEY_M: 0x10,
    KEY_N: 0x11,
    KEY_O: 0x12,
    KEY_P: 0x13,
    KEY_Q: 0x14,
    KEY_R: 0x15,
    KEY_S: 0x16,
    KEY_T: 0x17,
    KEY_U: 0x18,
    KEY_V: 0x19,
    KEY_W: 0x1A,
    KEY_X: 0x1B,
    KEY_Y: 0x1C,
    KEY_Z: 0x1D,
    KEY_1: 0x1E,
    KEY_2: 0x1F,
    KEY_3: 0x20,
    KEY_4: 0x21,
    KEY_5: 0x22,
    KEY_6: 0x23,
    KEY_7: 0x24,
    KEY_8: 0x25,
    KEY_9: 0x26,
    KEY_0: 0x27,
    KEY_ENTER: 0x28,
    KEY_ESCAPE: 0x29,
    KEY_BACKSPACE: 0x2A,
    KEY_TAB: 0x2B,
    KEY_SPACE: 0x2C,
    KEY_MINUS: 0x2D,
    KEY_EQUAL: 0x2E,
    KEY_BRACKET_LEFT: 0x2F,
    KEY_BRACKET_RIGHT: 0x30,
    KEY_BACKSLASH: 0x31,
    KEY_EUROPE_1: 0x32,
    KEY_SEMICOLON: 0x33,
    KEY_APOSTROPHE: 0x34,
    KEY_GRAVE: 0x35,
    KEY_COMMA: 0x36,
    KEY_PERIOD: 0x37,
    KEY_SLASH: 0x38,
    KEY_CAPS_LOCK: 0x39,
    KEY_F1: 0x3A,
    KEY_F2: 0x3B,
    KEY_F3: 0x3C,
    KEY_F4: 0x3D,
    KEY_F5: 0x3E,
    KEY_F6: 0x3F,
    KEY_F7: 0x40,
    KEY_F8: 0x41,
    KEY_F9: 0x42,
    KEY_F10: 0x43,
    KEY_F11: 0x44,
    KEY_F12: 0x45,
    KEY_PRINT_SCREEN: 0x46,
    KEY_SCROLL_LOCK: 0x47,
    KEY_PAUSE: 0x48,
    KEY_INSERT: 0x49,
    KEY_HOME: 0x4A,
    KEY_PAGE_UP: 0x4B,
    KEY_DELETE: 0x4C,
    KEY_END: 0x4D,
    KEY_PAGE_DOWN: 0x4E,
    KEY_ARROW_RIGHT: 0x4F,
    KEY_ARROW_LEFT: 0x50,
    KEY_ARROW_DOWN: 0x51,
    KEY_ARROW_UP: 0x52,
    KEY_NUM_LOCK: 0x53,
    KEY_KEYPAD_DIVIDE: 0x54,
    KEY_KEYPAD_MULTIPLY: 0x55,
    KEY_KEYPAD_SUBTRACT: 0x56,
    KEY_KEYPAD_ADD: 0x57,
    KEY_KEYPAD_ENTER: 0x58,
    KEY_KEYPAD_1: 0x59,
    KEY_KEYPAD_2: 0x5A,
    KEY_KEYPAD_3: 0x5B,
    KEY_KEYPAD_4: 0x5C,
    KEY_KEYPAD_5: 0x5D,
    KEY_KEYPAD_6: 0x5E,
    KEY_KEYPAD_7: 0x5F,
    KEY_KEYPAD_8: 0x60,
    KEY_KEYPAD_9: 0x61,
    KEY_KEYPAD_0: 0x62,
    KEY_KEYPAD_DECIMAL: 0x63,
    KEY_EUROPE_2: 0x64,
    KEY_APPLICATION: 0x65,
    KEY_POWER: 0x66,
    KEY_KEYPAD_EQUAL: 0x67,
    KEY_F13: 0x68,
    KEY_F14: 0x69,
    KEY_F15: 0x6A,
    KEY_F16: 0x6B,
    KEY_F17: 0x6C,
    KEY_F18: 0x6D,
    KEY_F19: 0x6E,
    KEY_F20: 0x6F,
    KEY_F21: 0x70,
    KEY_F22: 0x71,
    KEY_F23: 0x72,
    KEY_F24: 0x73,
    KEY_EXECUTE: 0x74,
    KEY_HELP: 0x75,
    KEY_MENU: 0x76,
    KEY_SELECT: 0x77,
    KEY_STOP: 0x78,
    KEY_AGAIN: 0x79,
    KEY_UNDO: 0x7A,
    KEY_CUT: 0x7B,
    KEY_COPY: 0x7C,
    KEY_PASTE: 0x7D,
    KEY_FIND: 0x7E,
    KEY_MUTE: 0x7F,
    KEY_VOLUME_UP: 0x80,
    KEY_VOLUME_DOWN: 0x81,
    KEY_LOCKING_CAPS_LOCK: 0x82,
    KEY_LOCKING_NUM_LOCK: 0x83,
    KEY_LOCKING_SCROLL_LOCK: 0x84,
    KEY_KEYPAD_COMMA: 0x85,
    KEY_KEYPAD_EQUAL_SIGN: 0x86,
    KEY_KANJI1: 0x87,
    KEY_KANJI2: 0x88,
    KEY_KANJI3: 0x89,
    KEY_KANJI4: 0x8A,
    KEY_KANJI5: 0x8B,
    KEY_KANJI6: 0x8C,
    KEY_KANJI7: 0x8D,
    KEY_KANJI8: 0x8E,
    KEY_KANJI9: 0x8F,
    KEY_LANG1: 0x90,
    KEY_LANG2: 0x91,
    KEY_LANG3: 0x92,
    KEY_LANG4: 0x93,
    KEY_LANG5: 0x94,
    KEY_LANG6: 0x95,
    KEY_LANG7: 0x96,
    KEY_LANG8: 0x97,
    KEY_LANG9: 0x98,
    KEY_ALTERNATE_ERASE: 0x99,
    KEY_SYSREQ_ATTENTION: 0x9A,
    KEY_CANCEL: 0x9B,
    KEY_CLEAR: 0x9C,
    KEY_PRIOR: 0x9D,
    KEY_RETURN: 0x9E,
    KEY_SEPARATOR: 0x9F,
    KEY_OUT: 0xA0,
    KEY_OPER: 0xA1,
    KEY_CLEAR_AGAIN: 0xA2,
    KEY_CRSEL_PROPS: 0xA3,
    KEY_EXSEL: 0xA4,
    KEY_CONTROL_LEFT: 0xE0,
    KEY_SHIFT_LEFT: 0xE1,
    KEY_ALT_LEFT: 0xE2,
    KEY_GUI_LEFT: 0xE3,
    KEY_CONTROL_RIGHT: 0xE4,
    KEY_SHIFT_RIGHT: 0xE5,
    KEY_ALT_RIGHT: 0xE6,
    KEY_GUI_RIGHT: 0xE7
};


class switchConfig{
    constructor(){
        this.macroBackgroundColor = 0x0000; //Black or nothing in a LED
        this.macroList = new Map();
        this.nameMacro = '';
        this.macroLastID = 0;
    }

    setMacroName(name){this.nameMacro = name.trim().toString();}

    getMacroName(){return this.nameMacro;}

    setBackgroundColor(colorCode){this.macroBackgroundColor = colorCode;}

    getBackgroundColor(){return this.macroBackgroundColor;}

    setMacroList(macroList){ 
    }

    getMacroList(){return this.macroList;}

    addMacro(macroValue)
    {
        this.macroList.set(this.macroLastID, macroValue);

        let macroID = this.macroLastID;
        this.macroLastID += 1;

        return macroID;
    }

    deleteMacro(macroID)
    {
        let deleted = this.macroList.delete(parseInt(macroID,10));
    }
}

class macroProfile{
    constructor(name){
        this.profileName = name;
        this.switchMap = new Map();
        this.backgroundEffect = 'Solid';
        this.backgroundEffectMainColor = 0x0000; //No color
        this.backgroundEffectSecColor = 0x0000;
    }

    setProfileName(name){this.profileName = name;}

    getProfileName(){return this.profileName;}

    setBackgroundEffect(effect, mainColor, secColor){
        this.backgroundEffect = effect;
        this.backgroundEffectMainColor = mainColor;
        this.backgroundEffectSecColor = secColor;
    }

    getBackgroundEffect(){return this.backgroundEffect;}

    getBackgroundMainColor(){return this.backgroundEffectMainColor;}

    getBackgroundSecColor(){return this.backgroundEffectSecColor;}

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

    getAllSwitchConfig(){
        return this.switchMap;
    }
}

function createKeyList()
{
    let switchKeyMacroSelect = document.getElementById("switchKeyMacroSelect");
    for (const key in keyDefines) {
        const option = document.createElement("option");
        option.value = key;
        option.text = key;
        switchKeyMacroSelect.appendChild(option);
    }
}

function addTask(){
    if(profileInputBox.value == ""){
        alert("Profile name empty, please add a name.");
    }
    else if(profilesMap.has(profileInputBox.value) == false){ //Check if we already register this value
        addProfileLabel(profileInputBox.value)

        //Instance a new profile and save into the map with profile name as key
        const profileInstance = new macroProfile(profileInputBox.value);
        profilesMap.set(profileInputBox.value, profileInstance);

        setProfileTitle(profileInputBox.value);
        

        //Save a default matrix naming
        for(i = 0; i < 12; i++)
        {
            const switchConfigInstance = new switchConfig();
            switchConfigInstance.setMacroName(profileInputBox.value + i);
            profileInstance.addSwitchConfig(i, switchConfigInstance);
        }
        
        updateButtonsNames(profileInputBox.value);
        updateSwitchMatrixColors();
        getBackgroundEffect(true); //Just to initialize by default
    }

    profileInputBox.value = "";
}

function addProfileLabel(profileName){
    let li = document.createElement("li");
    li.id = profileName;
    li.innerHTML = profileName;
    profileList.appendChild(li);

    let span = document.createElement("span");
    span.innerHTML = "\u00d7";
    li.appendChild(span);

    //Select the newest profile
    li.classList.toggle("checked");
    if(lastProfile != null){
        lastProfile.classList.toggle("checked");
    }

    lastProfile = li;
}

function setProfileTitle(profileName){
    let profileTitle = document.getElementById('matrixTitle');
    profileTitle.textContent = profileName;
}

function getProfileTitle(){
    let profileName = document.getElementById("matrixTitle");
    return profileName.innerHTML.toString();
}

function updateButtonsNames(profileName){
    let profileInfo = profilesMap.get(profileName);
    let buttons = document.getElementsByClassName("matrixButton");
    
    for (let index = 0; index < buttons.length; index++) {
        let switchConfig = profileInfo.getSwitchConfig(index.toString());
        buttons[index].innerHTML = switchConfig.getMacroName();

       // buttons[selectedSwitchID].style.boxShadow = "0 0 20px " +  switchBackground.value.toString();
    }
}

function updateProfileName()
{
    let newNameInputBox = document.getElementById("matrixNewNameInputBox");
    let originalProfileName = document.getElementById("matrixTitle");
    let listItemOriginal = document.getElementById(originalProfileName.innerHTML);

    if(newNameInputBox.value == ""){
        alert("New name empty, please add somenthing.");
        return;
    }
    else{
        if(profilesMap.has(newNameInputBox.value))
        {
            alert("This names is already in use, please use a new one.");
            return;
        }
        let originalProfileName = getProfileTitle();
        //Get original map entry, delete it, update name an reinsert.
        let oldProfile = profilesMap.get(originalProfileName);
        oldProfile.setProfileName(newNameInputBox.value);

        profilesMap.delete(originalProfileName);
        
        profilesMap.set(newNameInputBox.value, oldProfile);

        addProfileLabel(newNameInputBox.value);
        setProfileTitle(newNameInputBox.value);
        profileList.removeChild(listItemOriginal);
    }
    newNameInputBox.value = "";
}

function saveBackgroundEffect(){
    //Get profile Instance
    let profileName = getProfileTitle();
    let profile = profilesMap.get(profileName);

    //Get Colors selected;
    let colorPrim = document.getElementById("colorPrim");
    let colorSec =  document.getElementById("colorSec");

    //Save it
    profile.setBackgroundEffect(matrixBCKEffectsDropdown.value.toString(), colorPrim.value, colorSec.value);
}

function getBackgroundEffect(newProfile){
    //Get profile Instance
    let profileName = getProfileTitle();
    let profile = profilesMap.get(profileName);

    matrixBCKEffectsDropdown.value = profile.getBackgroundEffect();
    colorPrim.value = profile.getBackgroundMainColor();
    colorSec.value = profile.getBackgroundSecColor();

    backgroundColorVisible(matrixBCKEffectsDropdown.value);
}

function matrixButtonClick(id){
    //Make available switch config menu
    let macroList = document.getElementById("macroList");
    const switchConfigTitle = document.getElementById("switchConfigTitle");
    let switchBackground = document.getElementById("switchBackground");

    let profileName = getProfileTitle();
    let profile = profilesMap.get(profileName);

    //Get the switch base on ID
    let switchConfig = profile.getSwitchConfig(id.toString());

    //Update switch config tittle
    switchConfigTitle.innerHTML = switchConfig.getMacroName();

    //Set saved color
    let switchColor = switchConfig.getBackgroundColor();
    switchBackground.value = switchColor;

    // Get saved macroList
    macroList.innerHTML = "";
    let macroListMap = switchConfig.getMacroList();
    macroListMap.forEach((value, key) => {
        let li = document.createElement("li");
        li.id = key;
        li.innerHTML = value;
        macroList.appendChild(li);

        let span = document.createElement("span");
        span.innerHTML = "\u00d7";
        li.appendChild(span);
    });

    selectedSwitchID = id;

}

function updateMacroName(){
    if(selectedSwitchID == null) return;

    let profileName = getProfileTitle();
    let switchConfigInputBox = document.getElementById("switchConfigInputBox");

    let profile = profilesMap.get(profileName);

    let switchConfig = profile.getSwitchConfig(selectedSwitchID.toString());

    switchConfig.setMacroName(switchConfigInputBox.value);

    updateButtonsNames(profileName);

    switchConfigInputBox.value = "";
}

function updateSwitchColor(){
    let switchBackground = document.getElementById("switchBackground");
    let matrixButton = document.getElementsByClassName("matrixButton");
    let profileName = getProfileTitle();
    let switchColorEnable = document.getElementById("switchColorEnable");

    let profile = profilesMap.get(profileName);

    //Save on class the new color
    let switchConfig = profile.getSwitchConfig(selectedSwitchID.toString());
    switchConfig.setBackgroundColor(switchBackground.value);

    matrixButton[selectedSwitchID].style.boxShadow = "0 0 20px " +  switchBackground.value.toString();
    
}

function updateSwitchMatrixColors(){
    let switchBackground = document.getElementById("switchBackground");
    let matrixButton = document.getElementsByClassName("matrixButton");
    let profileName = getProfileTitle();
    let switchColorEnable = document.getElementById("switchColorEnable");

    let profile = profilesMap.get(profileName);

    //Set saved colors
    for(i = 0; i<12; i++)
    {
        let switchConfig = profile.getSwitchConfig(i.toString());
        matrixButton[i].style.boxShadow = "0 0 20px " +  switchConfig.getBackgroundColor();
    }
    
}

function convertTextToKeyCodes(text) {
    const keyCodes = [];

    for (const char of text) {
        const upperChar = char.toUpperCase();
        const keyCode = keyDefines[`KEY_${upperChar}`];

        if (keyCode !== undefined) {
            keyCodes.push(keyCode);
        }
    }

    return keyCodes;
}

function addMacro(isText){
    if (typeof isText !== 'boolean'){
        throw new Error('Argument must be boolean');
    };

    //Just to get profile class instance
    let profileName = getProfileTitle();
    let profile = profilesMap.get(profileName);

    //Save new macro.
    let switchConfig = profile.getSwitchConfig(selectedSwitchID.toString());
    let macroID = switchConfig.addMacro((isText) ? convertTextToKeyCodes(switchMacroTextInputBox.value) : keyDefines[switchKeyMacroSelect.value]);

    // Create a new input on the list
    let li = document.createElement("li");
    li.id = macroID;
    li.innerHTML = (isText) ? switchMacroTextInputBox.value : switchKeyMacroSelect.value;
    macroList.appendChild(li);

    let span = document.createElement("span");
    span.innerHTML = "\u00d7";
    li.appendChild(span);
}

function backgroundColorVisible(backgroundName){

    //Base on background style, let choose which color we could use.
    if(backgroundName == 'Solid')
    {
        colorPrim.style.display = 'block';
        colorSec.style.display = 'none';
    }
    else if(backgroundName == 'Fade')
    {
        colorPrim.style.display = 'block';
        colorSec.style.display = 'none';
    }
    else if(backgroundName == 'Rainbow')
    {
        colorPrim.style.display = 'none';
        colorSec.style.display = 'none';
    }
    else if(backgroundName == 'ColorWave')
    {
        colorPrim.style.display = 'block';
        colorSec.style.display = 'block';
    }
}

function macrosJSON(switchID){
    let profileName = getProfileTitle();
    let profile = profilesMap.get(profileName);
    let switchConfig = profile.getSwitchConfig(switchID.toString());
    let macroListSwitch = switchConfig.getMacroList();

    const macroList = [];
    macroListSwitch.forEach((value, key) => {
        macroList.push(value);
    });

    return macroList;
}

function switchMacrosJSON()
{
    let profileName = getProfileTitle();
    let profile = profilesMap.get(profileName);
    let switchMap = profile.getAllSwitchConfig();

    const switchConfigList = [];

    switchMap.forEach((value, key) => {
        const switchConfig = {
            switchCode: key,
            macroName: value.getMacroName(),
            pushColor: value.getBackgroundColor(),
            macroList: macrosJSON(key),
        };

        switchConfigList.push(switchConfig);
    });

    return switchConfigList;
}

function saveProfile(){
    let profileName = getProfileTitle();
    let profile = profilesMap.get(profileName);
    let switchMap = profile.getAllSwitchConfig();

    const profileData = {
        profileName: profileName,
        profileBackgroundEffect: profile.getBackgroundEffect(),
        profileMainColor: profile.getBackgroundMainColor(),
        profileSecColor: profile.getBackgroundSecColor(),
        switchMacros: switchMacrosJSON(),
    };

    console.log(JSON.stringify(profileData));

    fetch(profileName.toString(), {
    method: "POST",
    body: JSON.stringify(profileData),
    headers: {
        "Content-type": "application/json; charset=UTF-8"
    }
    })
    .then((response) => response.json())
    .then((json) => console.log(json));
}

profileList.addEventListener("click", function(e){
    if(e.target.tagName === "LI"){
        let profileName = e.target.textContent;
        profileName = profileName.slice(0, -1);
        setProfileTitle(profileName);

        updateButtonsNames(profileName);
        updateSwitchMatrixColors();
        getBackgroundEffect();

        //This is to avoid select to profiles at the same time.
        if(lastProfile != null){
            lastProfile.classList.toggle("checked");
        }

        e.target.classList.toggle("checked");
        lastProfile = e.target;
    }
    else if(e.target.tagName == "SPAN"){
        let profileName = e.target.parentElement.textContent;
        profileName = profileName.slice(0, -1); //Delete the last X

        if(profilesMap.has(profileName))
        {
            //TODO: Show an alert to be sure that you want to deleted this profile
            profilesMap.delete(profileName);
            e.target.parentElement.remove();
        }
    }
}, false);

macroList.addEventListener("click", function(e){
    if(e.target.tagName == "SPAN"){
        let macroID = e.target.parentElement.id;

        let profileName = getProfileTitle();
        let profile = profilesMap.get(profileName);

        let switchConfig = profile.getSwitchConfig(selectedSwitchID.toString());
        switchConfig.deleteMacro(macroID);
        e.target.parentElement.remove();
    }
}, false);

matrixBCKEffectsDropdown.addEventListener("click", function(e){
    backgroundColorVisible(e.target.value);
}, false);

createKeyList();


