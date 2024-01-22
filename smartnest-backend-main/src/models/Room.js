const mongoose = require("mongoose");
const relaySchema = require("./Relay");

const roomSchema = new mongoose.Schema({
  name: {
    type: String,
    required: true,
  },
  relays: [relaySchema],
});

module.exports = roomSchema;
