var icon;
var city;
var temp;

function getWeather(lat, long){
	var rawResponse;
	var apiKey = "ADD YOUR KEY HERE";
	var forecastRequest = new XMLHttpRequest();
	var geocodingRequest = new XMLHttpRequest();
	
	geocodingRequest.open('GET', 'http://maps.googleapis.com/maps/api/geocode/json?latlng=' + lat + ',' + long + '&sensor=true', true);
	geocodingRequest.onload = function(e) {
		if(geocodingRequest.readyState == 4 && geocodingRequest.status == 200)
		{
			var geoResponse = JSON.parse(geocodingRequest.responseText);
			var addressParts = geoResponse.results;
			city = "N/A";
			var breakFlag = false;
			for(h=0;h<addressParts.length; h++)
			{
				for(i=0; i<addressParts[h].address_components.length; i++) {
					for(j=0; j<addressParts[h].address_components[i].types.length; j++)
					{
						if(addressParts[h].address_components[i].types[j] == "sublocality")
						{
							city = addressParts[h].address_components[i].short_name;
							breakFlag = true;
							break;
						}
					}
					if(breakFlag == true)
					{
							break;
					}
				}
				if(breakFlag == true)
				{
					break;
				}
			}

			Pebble.sendAppMessage({
    		    '0' : icon,
				'1' : temp,
				'2' : city
				});
		}
	}
	
	forecastRequest.open('GET', 'https://api.forecast.io/forecast/' + apiKey + '/' + lat + ',' + long, true);
	forecastRequest.onload = function(e) {
		if(forecastRequest.readyState == 4 && forecastRequest.status == 200)
		{
			var forecastResponse = JSON.parse(forecastRequest.responseText);
			icon = forecastResponse.currently.icon;
			temp = Math.round(forecastResponse.currently.temperature) + "\u00B0F";
			//Is there a way that this can be done in parallel? That would be better.......
        	geocodingRequest.send(null);
		}
	}
	forecastRequest.send(null);
}

function locationSuccess(pos) {
    var coordinates = pos.coords;
    getWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
    console.warn('location error (' + err.code + '): ' + err.message);
    Pebble.sendAppMessage({
         "city" : "Loc Unavailable",
		 "temp" : "N/A",
    	 "icon" : "default"
    });
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 

Pebble.addEventListener("ready",
                        function(e) {
                          locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                        });