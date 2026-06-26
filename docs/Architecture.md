# Drone.Server.Fake

`PiSubmarine.Drone.Server.Fake` is a fake drone-side composition root for
operator-station development.

## Responsibility

This module owns:

- fake low-level control implementations needed to run the stack
- fake telemetry providers needed to expose a stable telemetry snapshot

It does not own:

- policy-level server wiring
- production control business logic
- production telemetry algorithms
- hardware drivers

## Design

`PiSubmarine.Drone.Server` now owns the policy-level composition root and the
public `Runtime` type. `PiSubmarine.Drone.Server.Fake` keeps the fake low-level
controllers, telemetry providers, and simulation pieces in `src`, while the app
instantiates those fake platform services and injects them into
`PiSubmarine::Drone::Server::Runtime`.
