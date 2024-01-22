const mongoose = require("mongoose");
const roomSchema = require("./Room");

const houseSchema = new mongoose.Schema({
  userId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: "User",
  },
  name: {
    type: String,
    required: true,
  },
  rooms: [roomSchema],
});

module.exports = houseSchema;

mongoose.model("House", houseSchema);
