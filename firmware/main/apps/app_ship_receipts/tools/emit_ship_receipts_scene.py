#!/usr/bin/env python3
import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
EXAMPLES = ROOT / "examples"

SCENES = {
    "heike": EXAMPLES / "heike_scene.json",
    "odyssey": EXAMPLES / "odyssey_scene.json",
}


def load_scene(name: str) -> dict:
    return json.loads(SCENES[name].read_text())


def build_output(scene: dict, fmt: str) -> dict:
    if fmt == "scene":
        return scene
    if fmt == "command":
        return {"cmd": "shipReceiptsScene", "data": scene}
    if fmt == "ws-text":
        return {
            "name": "ship-receipts",
            "content": json.dumps({"cmd": "shipReceiptsScene", "data": scene}, ensure_ascii=False),
        }
    raise ValueError(f"unknown format: {fmt}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Emit Ship Receipts app scene payloads for StackChan")
    parser.add_argument("--scene", choices=sorted(SCENES.keys()), default="heike")
    parser.add_argument("--format", choices=["scene", "command", "ws-text"], default="command")
    args = parser.parse_args()

    scene = load_scene(args.scene)
    payload = build_output(scene, args.format)
    print(json.dumps(payload, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
