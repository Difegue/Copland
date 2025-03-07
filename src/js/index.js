// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config.json');
// Initialize Clay
var clay = new Clay(clayConfig, function(minified) {
    var clayConfig = this;
    var _ = minified._;
    var $ = minified.$;
    var HTML = minified.HTML;
  
    function resetSettings() {
        clayConfig.getItemByMessageKey('showDate').set(true);
        clayConfig.getItemByMessageKey('showSeconds').set(true);
        clayConfig.getItemByMessageKey('removeBorder').set(false);
        clayConfig.getItemByMessageKey('corneredDate').set(false);
      
        clayConfig.getItemByMessageKey('bgColor').set('FFFFFF');
        clayConfig.getItemByMessageKey('hoursColor').set('00AA55');
        clayConfig.getItemByMessageKey('minutesColor').set('FF5555');
        clayConfig.getItemByMessageKey('secondsColor').set('FFAA00');
        clayConfig.getItemByMessageKey('dateColor').set('0055FF');
    }

    function colorPreset1() {
        clayConfig.getItemByMessageKey('bgColor').set('FFAAAA');
        clayConfig.getItemByMessageKey('hoursColor').set('AA0055');
        clayConfig.getItemByMessageKey('minutesColor').set('000055');
        clayConfig.getItemByMessageKey('secondsColor').set('AA00AA');
        clayConfig.getItemByMessageKey('dateColor').set('5555AA');
    }

    function colorPreset2() {
        clayConfig.getItemByMessageKey('bgColor').set('5555FF');
        clayConfig.getItemByMessageKey('hoursColor').set('FFFF00');
        clayConfig.getItemByMessageKey('minutesColor').set('FF5555');
        clayConfig.getItemByMessageKey('secondsColor').set('FFAA55');
        clayConfig.getItemByMessageKey('dateColor').set('FFAAFF');
    }

    function colorPreset3() {
        clayConfig.getItemByMessageKey('bgColor').set('FFFF55');
        clayConfig.getItemByMessageKey('hoursColor').set('FF00AA');
        clayConfig.getItemByMessageKey('minutesColor').set('55FFFF');
        clayConfig.getItemByMessageKey('secondsColor').set('55FF55');
        clayConfig.getItemByMessageKey('dateColor').set('550055');
    }
  
    clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
        clayConfig.getItemById('resetDefaults').on('click', resetSettings);
        clayConfig.getItemById('preset1').on('click', colorPreset1);
        clayConfig.getItemById('preset2').on('click', colorPreset2);
        clayConfig.getItemById('preset3').on('click', colorPreset3);
    });
    
  });

