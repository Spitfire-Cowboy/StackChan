/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

import 'package:flutter_test/flutter_test.dart';
import 'package:stack_chan/model/expression_data.dart';

void main() {
  group('ExpressionData', () {
    test('round-trips JSON for avatar payloads', () {
      final expression = ExpressionData(
        leftEye: ExpressionItem(x: 1, y: 2, rotation: 3, weight: 4, size: 5),
        rightEye: ExpressionItem(
          x: 6,
          y: 7,
          rotation: 8,
          weight: 9,
          size: 10,
        ),
        mouth: ExpressionItem(
          x: 11,
          y: 12,
          rotation: 13,
          weight: 14,
          size: 15,
        ),
      );

      final decoded = ExpressionData.fromJson(expression.toJson());

      expect(decoded.type, 'bleAvatar');
      expect(decoded.leftEye.toJson(), expression.leftEye.toJson());
      expect(decoded.rightEye.toJson(), expression.rightEye.toJson());
      expect(decoded.mouth.toJson(), expression.mouth.toJson());
      expect(expression.toString(), contains('"type":"bleAvatar"'));
    });

    test('copy returns a detached expression item', () {
      final original = ExpressionItem(x: 4, y: 5, rotation: 6, weight: 7, size: 8);
      final copy = original.copy();

      copy.x = 99;

      expect(original.x, 4);
      expect(copy.toJson(), isNot(original.toJson()));
      expect(copy.rotation, original.rotation);
      expect(copy.weight, original.weight);
      expect(copy.size, original.size);
    });
  });

  group('MotionData', () {
    test('prefers angle when serializing zero-rotate motions', () {
      final motion = MotionData(
        pitchServo: MotionDataItem(angle: 450, speed: 700),
        yawServo: MotionDataItem(angle: 120, speed: 500),
      );

      final decoded = MotionData.fromJson(motion.toJson());

      expect(decoded.pitchServo.angle, 450);
      expect(decoded.pitchServo.speed, 700);
      expect(decoded.yawServo.toJson(), {'angle': 120, 'speed': 500});
    });

    test('serializes rotate motions without an angle field', () {
      final item = MotionDataItem(rotate: 90, speed: 333);

      expect(item.toJson(), {'rotate': 90, 'speed': 333});
    });
  });

  group('RgbData', () {
    test('uses documented defaults and round-trips JSON', () {
      final rgb = RgbData();
      final decoded = RgbData.fromJson(rgb.toJson());

      expect(decoded.leftRgbColor, '#FFFFFF');
      expect(decoded.rightRgbColor, '#FFFFFF');
      expect(decoded.leftRgbDuration, 0.0);
      expect(decoded.rightRgbDuration, 0.0);
    });
  });
}
