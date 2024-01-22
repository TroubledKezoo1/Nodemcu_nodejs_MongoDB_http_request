require("./models/User");
require("./models/House");
require("dotenv").config();
const express = require("express");
const mongoose = require("mongoose");
const bodyParser = require("body-parser");
const authRoutes = require("./routes/authRoutes");
const houseRoutes = require("./routes/houseRoutes");
const requireAuth = require("./middlewares/requireAuth");

const app = express();

app.use(bodyParser.json());
app.use(authRoutes);
app.use(houseRoutes);

const mongoUri =
  "mongodb+srv://admin:sTC1COvmIgyMWlUB@backend-local.xewh5ga.mongodb.net/smartnest?retryWrites=true&w=majority";

mongoose.connect(mongoUri);

mongoose.connection.on("connected", () => {
  console.log("Connected to mongo instance");
});

mongoose.connection.on("error", (error) => {
  console.log("Error connecting to mongo", error);
});

app.get("/", requireAuth, (req, res) => {
  res.send(`Hello ${req.user.email}`);
});

app.listen(3000, () => {
  console.log("Listening on port 3000");
});
