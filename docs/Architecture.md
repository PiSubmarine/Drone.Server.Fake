# Drone.Server.Fake

`PiSubmarine.Drone.Server.Fake` is a fake drone-side composition root for
operator-station development.

## Responsibility

This module owns:

- wiring of lease, control, telemetry, and UDP/grpc transport modules
- fake low-level control implementations needed to run the stack
- fake telemetry providers needed to expose a stable telemetry snapshot

It does not own:

- production control business logic
- production telemetry algorithms
- hardware drivers

## Design

The public surface is intentionally small: one `Runtime` type with lifecycle
methods and a blocking `Run()` loop.

Internally it composes:

- `Lease.InMemory`
- `Grpc.Server`
- `Lease.Server.Grpc`
- `Video.Subscription.Grpc.Server`
- `Control.Engine`
- `Control.Gimbal.Servo`
- `Control.Server.Udp`
- `Control.Pilot.Manual`
- `Control.Pilot.Dummy`
- `Battery.Telemetry.Protobuf`
- `Motor.Telemetry.Protobuf`
- `Telemetry.Server.Udp`
- `Time.Manager`
- `Udp.Asio`

`Runtime` owns the static composition graph, binds the UDP sockets, starts the
shared gRPC server, and then gives the main thread to `Time.Manager`.

For gimbal control, `Runtime` composes the real `Control.Gimbal.Servo`
implementation on top of a fake private `Servo::IController`.

The fake low-level controllers and telemetry providers live only inside this
module as private implementation details and are not intended to be reused
elsewhere.
