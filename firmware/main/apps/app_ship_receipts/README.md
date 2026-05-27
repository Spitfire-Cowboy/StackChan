# Ship Receipts app shell

This directory is the first explicit app boundary for integrating `ship-receipts`
with StackChan.

## Intent

`ship-receipts` should be an **app on top of StackChan core**, not logic wired
into generic firmware subsystems.

That means:

- keep **transport**, **motion**, **audio**, **diagnostics**, and **renderer
  primitives** in StackChan core
- keep **mode/content logic**, **copy**, **scene sequencing**, and
  `ship-receipts` semantics in this app layer

## Why this matters

This split gives us:

- a clean place to build host-driven game modes
- better odds of upstreaming generic capabilities later
- less pressure to turn StackChan core into a `ship-receipts`-specific product

## What belongs here later

- host scene packet consumers for Ship Receipts modes
- Odyssey / Heike / future mode selection
- app-local diagnostics relevant to Ship Receipts mode
- branding and copy specific to this app
- app-level use of generic core capabilities such as:
  - avatar skin + speech
  - motion updates
  - audio cue playback
  - transport-fed scene packets

## What should stay out

- generic servo transport code
- generic audio plumbing
- generic small-screen layout helpers
- generic packet logging / debugging

## Current shell behavior

The current shell is still intentionally small, but it now demonstrates an
app-local "director" loop that uses generic StackChan core capabilities:

- opens as a normal StackChan app
- attaches the default avatar
- rotates through a few Ship Receipts sample beats
- uses:
  - status text for the beat label
  - avatar speech for the beat line
  - built-in emotions for expression
  - head motion for staging
  - RGB color changes for mood
  - built-in notification audio for cueing
- uses the standard home indicator and status bar

This keeps the seam app-local while proving a stronger point than the original
stub: Ship Receipts can direct StackChan through existing core APIs without
turning the core firmware into a Ship Receipts-specific fork.
