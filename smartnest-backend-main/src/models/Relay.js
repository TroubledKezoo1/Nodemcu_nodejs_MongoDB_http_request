const mongoose = require("mongoose");

const relaySchema = mongoose.Schema({
  name: {
    type: String,
    required: true,
  },
  status: {
    type: String,
    required: true,
  },
});

module.exports = relaySchema;
