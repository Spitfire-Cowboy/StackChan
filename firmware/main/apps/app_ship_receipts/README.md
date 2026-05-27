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

## What should stay out

- generic servo transport code
- generic audio plumbing
- generic small-screen layout helpers
- generic packet logging / debugging

