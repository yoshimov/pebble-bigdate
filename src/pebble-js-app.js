Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'https://rawgit.com/yoshimov/93b4e31c5df00e48f7b8/raw/pebble-bigdate.html';
  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  var secondColor = configData['second_color'];
  var dateColor = configData['date_color'];
  var hourColor = configData['hour_color'];

  var dict = {};
  dict['KEY_SECOND'] = parseInt(configData['second']);
  dict['KEY_SECOND_COLOR'] = parseInt(secondColor.substring(2), 16);
  dict['KEY_DATE_COLOR'] = parseInt(dateColor.substring(2), 16);
  if (hourColor) {
    dict['KEY_HOUR_COLOR'] = parseInt(hourColor.substring(2), 16);
  }
  console.log('dict: ' + JSON.stringify(dict));

  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
});