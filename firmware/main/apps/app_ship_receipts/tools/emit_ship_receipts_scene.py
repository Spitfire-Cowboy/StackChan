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
    if fmt in {"command", "ble-config"}:
        return {"cmd": "shipReceiptsScene", "data": scene}
    if fmt == "ws-text":
        return {
            "name": "ship-receipts",
            "content": json.dumps({"cmd": "shipReceiptsScene", "data": scene}, ensure_ascii=False),
        }
    if fmt == "transport-note":
        return {
            "ble": {
                "service_uuid": "e2e5e5e0-1234-5678-1234-56789abcdef0",
                "characteristic_uuid": "e2e5e5e3-1234-5678-1234-56789abcdef0",
                "shape": "ble-config",
            },
            "websocket": {
                "surface": "text-message",
                "sender_name": "ship-receipts",
                "shape": "ws-text",
            },
        }
    if fmt == "metadata":
        return {
            "id": scene.get("id", ""),
            "mode": scene.get("mode", ""),
            "presentation_type": scene.get("presentation_type", ""),
            "visual_template": scene.get("visual_template", ""),
        }
    raise ValueError(f"unknown format: {fmt}")


def main() -> None:
    parser = argparse.ArgumentParser(description="Emit Ship Receipts app scene payloads for StackChan")
    parser.add_argument("--scene", choices=sorted(SCENES.keys()), default="heike")
    parser.add_argument(
        "--format",
        choices=["scene", "command", "ble-config", "ws-text", "transport-note", "metadata"],
        default="command",
    )
    args = parser.parse_args()

    scene = load_scene(args.scene)
    payload = build_output(scene, args.format)
    print(json.dumps(payload, ensure_ascii=False, indent=2))


if __name__ == "__main__":
    main()
