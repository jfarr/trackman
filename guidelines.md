# Guidelines

## Design

- Views own static child views
- Controllers can reference views
- Controllers can own dynamic child views
- Views can't reference controllers
  - Use callbacks to send messages from views to controllers
- Views are only referenced by one controller
  - Use callbacks between controllers instead of multiple controllers sending messages to the same view
- Controllers can reference or own other controllers
- All code in namespace "trackman"
- Use "using namespace std" and "using namespace juce"

## Implementation

- Use `unique_ptr` / `shared_ptr`
