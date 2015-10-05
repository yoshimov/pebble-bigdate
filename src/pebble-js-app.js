Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'https://rawgit.com/yoshimov/pebble-bigdate/master/config/config.html';
  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  var dict = {};
  dict['KEY_SECOND'] = parseInt(configData['second'], 10);
  dict['KEY_SECOND_COLOR'] = parseInt(configData['second_color'].substring(2), 16);
  dict['KEY_DATE_COLOR'] = parseInt(configData['date_color'].substring(2), 16);
  if (configData['hour_color']) {
    dict['KEY_HOUR_COLOR'] = parseInt(configData['hour_color'].substring(2), 16);
  }
  if (configData['minute_color']) {
    dict['KEY_MINUTE_COLOR'] = parseInt(configData['minute_color'].substring(2), 16);
  }
  if (configData['background']) {
    dict['KEY_BACKGROUND'] = parseInt(configData['background'], 10);
  }
  if (configData['date_size']) {
    dict['KEY_DATE_SIZE'] = parseInt(configData['date_size'], 10);
  }
  if (configData['week_size']) {
    dict['KEY_WEEK_SIZE'] = parseInt(configData['week_size'], 10);
  }
  if (configData['week_style']) {
    dict['KEY_WEEK_STYLE'] = parseInt(configData['week_style'], 10);
  }
  console.log('dict: ' + JSON.stringify(dict));

  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
});