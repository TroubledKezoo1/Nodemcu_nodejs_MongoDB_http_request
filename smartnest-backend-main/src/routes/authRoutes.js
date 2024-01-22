const express = require("express");
const jwt = require("jsonwebtoken");
const mongoose = require("mongoose");
const { validationResult } = require("express-validator");

const User = mongoose.model("User");

const router = express.Router();

router.post("/signup", async (req, res) => {
  const { email, password } = req.body;

  try {
    const errors = validationResult(req);

    if (!errors.isEmpty()) {
      return res.status(422).json({ errors: errors.array() });
    }

    const user = new User({ email, password });
    await user.save();

    // JWT oluşturma
    const payload = {
      userId: user._id,
    };

    const token = jwt.sign(payload, process.env.ACCESS_TOKEN_SECRET, {
      expiresIn: "1h",
    });

    res.send({ token });
  } catch (error) {
    return res.status(500).send({ error: "Server error" });
  }
});

router.post("/signin", async (req, res) => {
  const { email, password } = req.body;

  if (!email || !password) {
    return res.status(422).send({ error: "Must provide email and password." });
  }

  const user = await User.findOne({ email });

  if (!user) {
    return res.status(404).send({ error: "Invalid password or email." });
  }

  try {
    await user.comparePassword(password);

    const payload = {
      userId: user._id,
    };

    const token = jwt.sign(payload, process.env.ACCESS_TOKEN_SECRET, {
      expiresIn: "1h",
    });
    res.send({ token, userId: payload.userId });
  } catch (error) {
    return res.status(422).send({ error: "Invalid password or email." });
  }
});

module.exports = router;
