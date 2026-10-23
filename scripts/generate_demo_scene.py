#!/usr/bin/env python3
"""
Writes assets/scenes/Demo.mxscene: the showcase scene that the editor and the
Sandbox open by default. The scene file format is plain JSON, so you can also
edit it by hand or in the editor; this script just makes regenerating it easy.

    python scripts/generate_demo_scene.py
"""
import json
import os
import random

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), os.pardir))
OUT = os.path.join(ROOT, "assets", "scenes", "Demo.mxscene")

random.seed(20240101)
entities = []


def uuid():
    return random.getrandbits(63) | 1


def entity(tag, position=(0, 0, 0), rotation=0.0, scale=(1, 1, 1), **components):
    e = {"ID": uuid(), "Tag": tag, "TransformComponent": {"Position": list(position), "Rotation": rotation, "Scale": list(scale)}}
    e.update(components)
    entities.append(e)
    return e


def parent(parent_entity, *children):
    parent_entity.setdefault("RelationshipComponent", {"Parent": 0, "Children": []})
    for child in children:
        child.setdefault("RelationshipComponent", {"Parent": 0, "Children": []})
        child["RelationshipComponent"]["Parent"] = parent_entity["ID"]
        parent_entity["RelationshipComponent"]["Children"].append(child["ID"])


def sprite(color=(1, 1, 1, 1), texture="", tiling=(1.0, 1.0)):
    return {"Color": list(color), "TexturePath": texture, "Tiling": list(tiling)}


def rigidbody(body_type="Static", mass=1.0, restitution=0.0, friction=0.4, gravity=1.0, damping=0.0):
    return {"Type": body_type, "Velocity": [0, 0], "GravityScale": gravity, "Mass": mass, "Restitution": restitution,
            "Friction": friction, "LinearDamping": damping, "FixedRotation": True}


def box(size=(0.5, 0.5), offset=(0, 0), trigger=False):
    return {"Offset": list(offset), "Size": list(size), "IsTrigger": trigger}


def circle_collider(radius=0.5, offset=(0, 0)):
    return {"Offset": list(offset), "Radius": radius, "IsTrigger": False}


def script(name, **params):
    return {"ScriptName": name, "Parameters": params}


# --- Background ------------------------------------------------------------------------------
entity("Sky", (0, 4, -20), scale=(80, 40, 1), SpriteComponent=sprite(texture="assets/textures/sky.png"))

clouds = entity("Clouds", (0, 7.5, -15), NativeScriptComponent=script("Oscillator", Amplitude=3.0, Frequency=0.05, Horizontal=1.0))
parent(clouds,
       entity("Cloud A", (-9, 0.5, 0), scale=(4, 2, 1), SpriteComponent=sprite((1, 1, 1, 0.9), "assets/textures/cloud.png")),
       entity("Cloud B", (-1, 1.5, 0), scale=(5, 2.5, 1), SpriteComponent=sprite((1, 1, 1, 0.8), "assets/textures/cloud.png")),
       entity("Cloud C", (8, 0, 0), scale=(3.5, 1.75, 1), SpriteComponent=sprite((1, 1, 1, 0.85), "assets/textures/cloud.png")))

# --- Level geometry --------------------------------------------------------------------------
entity("Ground", (0, -4.5, 0), scale=(60, 1, 1),
       SpriteComponent=sprite(texture="assets/textures/ground.png", tiling=(60.0, 1.0)),
       RigidBody2DComponent=rigidbody("Static", friction=0.6),
       BoxCollider2DComponent=box())

platforms = entity("Platforms", (0, 0, 0))
parent(platforms,
       entity("Platform Left", (-8, -1.5, 0), scale=(4, 0.6, 1), SpriteComponent=sprite(texture="assets/textures/platform.png", tiling=(2.0, 1.0)),
              RigidBody2DComponent=rigidbody("Static"), BoxCollider2DComponent=box()),
       entity("Platform Middle", (-2, 0.5, 0), scale=(3, 0.6, 1), SpriteComponent=sprite(texture="assets/textures/platform.png", tiling=(1.5, 1.0)),
              RigidBody2DComponent=rigidbody("Static"), BoxCollider2DComponent=box()),
       entity("Platform High", (4, 2.5, 0), scale=(3, 0.6, 1), SpriteComponent=sprite(texture="assets/textures/platform.png", tiling=(1.5, 1.0)),
              RigidBody2DComponent=rigidbody("Static"), BoxCollider2DComponent=box()),
       entity("Moving Platform", (10, 0.0, 0), scale=(3, 0.6, 1), SpriteComponent=sprite((0.8, 0.9, 1.0, 1), "assets/textures/platform.png", (1.5, 1.0)),
              RigidBody2DComponent=rigidbody("Static"), BoxCollider2DComponent=box(),
              NativeScriptComponent=script("Oscillator", Amplitude=2.0, Frequency=0.2, Horizontal=0.0)))

# --- Player ----------------------------------------------------------------------------------
entity("Player", (-11, -2.5, 1),
       SpriteComponent=sprite(texture="assets/textures/player.png"),
       RigidBody2DComponent=rigidbody("Dynamic", mass=1.0, restitution=0.0, friction=0.2),
       BoxCollider2DComponent=box((0.42, 0.5)),
       NativeScriptComponent=script("PlatformerController2D", Speed=6.0, JumpForce=9.5, RespawnY=-20.0),
       AudioSourceComponent={"ClipPath": "assets/audio/jump.wav", "Volume": 0.8, "Pitch": 1.0, "Loop": False, "PlayOnStart": False})

# --- Physics playground ----------------------------------------------------------------------
crates = entity("Crates", (0, 0, 0))
parent(crates,
       *[entity(f"Crate {i + 1}", (1.0 + (i % 2) * 1.05, -3.5 + (i // 2) * 1.05, 0.5),
                SpriteComponent=sprite(texture="assets/textures/crate.png"),
                RigidBody2DComponent=rigidbody("Dynamic", mass=2.0, restitution=0.05, friction=0.7),
                BoxCollider2DComponent=box()) for i in range(6)])

balls = entity("Balls", (0, 0, 0))
parent(balls,
       *[entity(f"Ball {i + 1}", (5.5 + i * 0.9, 6.0 + i * 1.2, 0.5), scale=(0.7, 0.7, 1),
                CircleRendererComponent={"Color": [0.3 + i * 0.2, 0.7, 1.0 - i * 0.2, 1.0], "Thickness": 1.0, "Fade": 0.01},
                RigidBody2DComponent=rigidbody("Dynamic", mass=0.5, restitution=0.65, friction=0.2),
                CircleCollider2DComponent=circle_collider()) for i in range(4)])

# --- Collectibles (hierarchy + scripts) ------------------------------------------------------
coins = entity("Coins", (0, 0, 0))
parent(coins,
       *[entity(f"Coin {i + 1}", (-8 + i * 2.2, -0.2 + (i % 2) * 0.6 + (2.3 if i >= 5 else 0), 0.2), scale=(0.6, 0.6, 1),
                SpriteComponent=sprite(texture="assets/textures/coin.png"),
                NativeScriptComponent=script("Oscillator", Amplitude=0.15, Frequency=1.0 + 0.1 * i, Horizontal=0.0)) for i in range(7)])

# --- Windmill: rotating parent with child blades --------------------------------------------
windmill = entity("Windmill", (-5.5, 3.2, -1), NativeScriptComponent=script("Rotator", Speed=45.0))
hub = entity("Hub", (0, 0, 0.1), scale=(0.5, 0.5, 1), CircleRendererComponent={"Color": [0.35, 0.35, 0.4, 1.0], "Thickness": 1.0, "Fade": 0.02})
blades = [entity(f"Blade {i + 1}", (0, 0, 0), rotation=i * 90.0, scale=(0.5, 2.2, 1),
                 SpriteComponent=sprite(texture="assets/textures/blade.png")) for i in range(4)]
for i, b in enumerate(blades):
    # Offset each blade so it extends from the hub outwards.
    angle = i * 90.0
    import math
    b["TransformComponent"]["Position"] = [-math.sin(math.radians(angle)) * 1.1, math.cos(math.radians(angle)) * 1.1, 0]
parent(windmill, hub, *blades)
entity("Windmill Pole", (-5.5, 0.0, -2), scale=(0.3, 6.4, 1), SpriteComponent=sprite((0.45, 0.35, 0.3, 1)))

# --- Branding billboard ----------------------------------------------------------------------
entity("Matrix Logo", (13.0, 4.5, -5), scale=(7.5, 3.0, 1), SpriteComponent=sprite((1, 1, 1, 0.95), "resources/branding/Artboard 2.png"))

# --- Camera and audio ------------------------------------------------------------------------
entity("Main Camera", (-6, 0.5, 0),
       CameraComponent={"OrthographicSize": 13.0, "Near": -100.0, "Far": 100.0, "Primary": True, "FixedAspectRatio": False},
       NativeScriptComponent=script("CameraFollow2D", Smoothing=3.0, OffsetX=1.5, OffsetY=2.5))

entity("Ambient Audio", (0, 0, 0),
       AudioSourceComponent={"ClipPath": "assets/audio/ambient_loop.wav", "Volume": 0.25, "Pitch": 1.0, "Loop": True, "PlayOnStart": True})

scene = {
    "Scene": "Demo",
    "Version": 1,
    "Gravity": [0.0, -22.0],
    "BackgroundColor": [0.09, 0.1, 0.14, 1.0],
    "Entities": entities,
}

os.makedirs(os.path.dirname(OUT), exist_ok=True)
with open(OUT, "w", encoding="utf-8") as f:
    json.dump(scene, f, indent=2)
print(f"wrote {OUT} ({len(entities)} entities)")
