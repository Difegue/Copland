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
  
    clayConfig.on(clayConfig.EVENTS.AFTER_BUILD, function() {
        var resetSettingsBtn = clayConfig.getItemById('resetDefaults');
        resetSettingsBtn.on('click', resetSettings);
    });
    
  });

