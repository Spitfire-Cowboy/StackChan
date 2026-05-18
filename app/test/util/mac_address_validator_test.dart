/*
SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
SPDX-License-Identifier: MIT
*/

import 'package:flutter_test/flutter_test.dart';
import 'package:stack_chan/util/mac_address_validator.dart';

void main() {
  group('MacAddressValidator.isValidMac', () {
    test('accepts supported formats', () {
      expect(MacAddressValidator.isValidMac('00:11:22:33:44:55'), isTrue);
      expect(MacAddressValidator.isValidMac('aa-bb-cc-dd-ee-ff'), isTrue);
      expect(MacAddressValidator.isValidMac('AABBCCDDEEFF'), isTrue);
    });

    test('rejects malformed values', () {
      expect(MacAddressValidator.isValidMac(null), isFalse);
      expect(MacAddressValidator.isValidMac(''), isFalse);
      expect(MacAddressValidator.isValidMac('GG:11:22:33:44:55'), isFalse);
      expect(MacAddressValidator.isValidMac('00112233445'), isFalse);
    });
  });

  group('MacAddressValidator formatting helpers', () {
    test('normalize equivalent addresses to a stable form', () {
      expect(
        MacAddressValidator.formatMac('aa-bb-cc-dd-ee-ff'),
        'AA:BB:CC:DD:EE:FF',
      );
      expect(
        MacAddressValidator.formatLowerCaseMac('AABBCCDDEEFF'),
        'aa:bb:cc:dd:ee:ff',
      );
      expect(MacAddressValidator.toPureMac('aa:bb:cc:dd:ee:ff'), 'AABBCCDDEEFF');
      expect(MacAddressValidator.normalize('AA-BB-CC-DD-EE-FF'), 'aabbccddeeff');
    });

    test('compare addresses independent of case and separator', () {
      expect(
        MacAddressValidator.areEqual('aa:bb:cc:dd:ee:ff', 'AABBCCDDEEFF'),
        isTrue,
      );
      expect(
        MacAddressValidator.areEqual('aa:bb:cc:dd:ee:ff', '00:11:22:33:44:55'),
        isFalse,
      );
    });
  });
}
