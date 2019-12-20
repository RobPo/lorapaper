/*
// This is the code you will need to add to the 'encoder' section in the TTN backend to let the
// weather forecast demo run properly.

function Encoder(object, port) {
  // Encode downlink messages sent as
  // object to an array or buffer of bytes.
  var bytes = [9];

  bytes[0] = parseInt(object.temperature);
  bytes[2] = parseInt(object.humidity);
  bytes[4] = parseInt(object.date);
  bytes[6] = parseInt(object.hour);
  bytes[8] = parseInt(object.mins);
  bytes[10] = parseInt(object.icon);
  
  bytes[12] = parseInt(object.rainPrecipProb0);
  bytes[14] = parseInt(object.rainPrecipProb1);
  bytes[16] = parseInt(object.rainPrecipProb2); 
  bytes[18] = parseInt(object.rainPrecipProb3);
  bytes[20] = parseInt(object.rainPrecipProb4);
  bytes[22] = parseInt(object.rainPrecipProb5);
  bytes[24] = parseInt(object.rainPrecipProb6);
  bytes[26] = parseInt(object.rainPrecipProb7);
  bytes[28] = parseInt(object.rainPrecipProb8);
  bytes[30] = parseInt(object.rainPrecipProb9);
  bytes[32] = parseInt(object.rainPrecipProb10);
  bytes[34] = parseInt(object.rainPrecipProb11);

  return bytes;
}
*/
