const express = require("express");
const mongoose = require("mongoose");
const requireAuth = require("../middlewares/requireAuth");

const House = mongoose.model("House");
const router = express.Router();

router.use(requireAuth);
//Kullanıcıya ait bütün evleri getirmek için kullanılan
router.get("/houses/:userId", async (req, res) => {
  try {
    const houses = await House.find({ userId: req.params.userId });

    res.send(houses);
  } catch (error) {
    res.status(500).send({ error: "Internal server error" });
  }
});
//Kullanıcıya ait istenen / bir  evi getirmek için kullanılan
router.get("/houses/:userId/:houseId", async (req, res) => {
  try {
    const house = await House.findOne({
      _id: req.params.houseId,
      userId: req.params.userId,
    });

    if (!house) {
      return res.status(404).send({ error: "House not found" });
    }

    res.send(house);
  } catch (error) {
    res.status(500).send({ error: "Internal server error" });
  }
});
//Kullanıcıya ait istenen / bir  evin içindeki odaları görüntülemek için kullanılan
router.get("/houses/:userId/:houseId/rooms", async (req, res) => {
  try {
    const house = await House.findOne({
      _id: req.params.houseId,
      userId: req.params.userId,
    });

    if (!house) {
      return res.status(404).send({ error: "House not found" });
    }

    const rooms = house.rooms;

    res.send(rooms);
  } catch (error) {
    res.status(500).send({ error: "Internal server error" });
  }
});
//Kullanıcıya ait istenen / bir  evin içindeki istenen / bir  odayı göröntölemek için kullanılan
router.get("/houses/:userId/:houseId/rooms/:roomId", async (req, res) => {
  try {
    const house = await House.findOne({
      _id: req.params.houseId,
      userId: req.params.userId,
    });

    if (!house) {
      return res.status(404).send({ error: "House not found" });
    }

    const room = house.rooms.id(req.params.roomId);

    if (!room) {
      return res.status(404).send({ error: "Room not found" });
    }

    res.send(room);
  } catch (error) {
    res.status(500).send({ error: "Internal server error" });
  }
});
//Kullanıcıya ait istenen / bir  evin içindeki istenen / bir  odanın içindeki röleleri göröntölemek için kullanılan
router.get(
  "/houses/:userId/:houseId/rooms/:roomId/relays",
  async (req, res) => {
    try {
      const house = await House.findOne({
        _id: req.params.houseId,
        userId: req.params.userId,
      });

      if (!house) {
        return res.status(404).send({ error: "House not found" });
      }

      const room = house.rooms.id(req.params.roomId);

      if (!room) {
        return res.status(404).send({ error: "Room not found" });
      }

      const relays = room.relays;

      res.send(relays);
    } catch (error) {
      res.status(500).send({ error: "Internal server error" });
    }
  }
);
//Kullanıcıya ait istenen / bir  evin içindeki istenen / bir  odanın içindeki istenen / bir   rölelerin içindeki bir /istenen röleyi görüntülemek için
router.get(
  "/houses/:userId/:houseId/rooms/:roomId/relays/:relayId",
  async (req, res) => {
    try {
      const house = await House.findOne({
        _id: req.params.houseId,
        userId: req.params.userId,
      });

      if (!house) {
        return res.status(404).send({ error: "House not found" });
      }

      const room = house.rooms.id(req.params.roomId);

      if (!room) {
        return res.status(404).send({ error: "Room not found" });
      }

      const relay = room.relays.id(req.params.relayId);

      if (!relay) {
        return res.status(404).send({ error: "Relay not found" });
      }

      res.send(relay);
    } catch (error) {
      res.status(500).send({ error: "Internal server error" });
    }
  }
);
//Kullanıcıya ait ev eklemek için kullanılan
router.post("/houses", async (req, res) => {
  const { name, rooms } = req.body;

  if (!name || !rooms) {
    return res.status(422).send({ error: "You must provide a name and rooms" });
  }

  try {
    const house = new House({ name, rooms, userId: req.user._id });

    await house.save();

    res.send(house);
  } catch (error) {
    res.status(422).send({ error: error.message });
  }
});
//Kullanıcıya ait istenen / bir  ev eklemek için kullanılan
router.post("/houses/:userId/:houseId/rooms", async (req, res) => {
  try {
    const userId = req.params.userId;
    const houseId = req.params.houseId;
    const { name, relays } = req.body;

    // Kullanıcının seçtiği evi bul
    const house = await House.findOne({
      userId: userId,
      _id: houseId,
    });

    if (!house) {
      return res.status(404).send({ error: "House not found" });
    }

    // Yeni oda oluştur
    const newRoom = {
      name: name,
      relays: relays,
    };

    // Evdeki odalar dizisine yeni odayı ekle
    house.rooms.push(newRoom);

    // Değişiklikleri kaydet
    await house.save();

    // Eklenen oda bilgilerini yanıt olarak gönder
    res.send(newRoom);
  } catch (error) {
    res.status(500).send({ error: "Internal server error" });
  }
});
//Kullanıcıya ait istenen / bir  evin içindeki istenen / bir  odayın içindeki istenen / bir   rölelerin eklemek için kullanılan
router.post(
  "/houses/:userId/:houseId/rooms/:roomId/relays",
  async (req, res) => {
    try {
      const userId = req.params.userId;
      const houseId = req.params.houseId;
      const roomId = req.params.roomId;
      const { name, status } = req.body;

      // Kullanıcının seçtiği evi ve odasını bul
      const house = await House.findOne({
        userId: userId,
        _id: houseId,
        "rooms._id": roomId,
      });

      if (!house) {
        return res.status(404).send({ error: "House or room not found" });
      }

      // Seçtiğiniz odanın indexini bulun
      const roomIndex = house.rooms.findIndex(
        (room) => room._id.toString() === roomId
      );

      if (roomIndex === -1) {
        return res.status(404).send({ error: "Room not found" });
      }

      // Yeni röle oluştur
      const newRelay = {
        name: name,
        status: status,
      };

      // Odadaki röleler dizisine yeni röleyi ekle
      house.rooms[roomIndex].relays.push(newRelay);

      // Değişiklikleri kaydet
      await house.save();

      // Eklenen röle bilgilerini yanıt olarak gönder
      res.send(newRelay);
    } catch (error) {
      res.status(500).send({ error: "Internal server error" });
    }
  }
);

router.delete(
  "/houses/:userId/:houseId/rooms/:roomId/relays/:relayId",
  async (req, res) => {
    try {
      const userId = req.params.userId;
      const houseId = req.params.houseId;
      const roomId = req.params.roomId;
      const relayId = req.params.relayId;

      // Kullanıcının seçtiği evi ve odasını bul
      const house = await House.findOne({
        userId: userId,
        _id: houseId,
        "rooms._id": roomId,
      });

      if (!house) {
        return res.status(404).send({ error: "House or room not found" });
      }

      // Seçtiğiniz odanın indexini bulun
      const roomIndex = house.rooms.findIndex(
        (room) => room._id.toString() === roomId
      );

      if (roomIndex === -1) {
        return res.status(404).send({ error: "Room not found" });
      }

      // Seçtiğiniz rölenin indexini bulun
      const relayIndex = house.rooms[roomIndex].relays.findIndex(
        (relay) => relay._id.toString() === relayId
      );

      if (relayIndex === -1) {
        return res.status(404).send({ error: "Relay not found" });
      }

      // Röleyi sil
      house.rooms[roomIndex].relays.splice(relayIndex, 1);

      // Değişiklikleri kaydet
      await house.save();

      res.send({ message: "Relay deleted successfully" });
    } catch (error) {
      res.status(500).send({ error: "Internal server error" });
    }
  }
);

module.exports = router;
