# C Parser with PCRE2

Since no one ever has written a C parser the easy way (AKA with RegEx) I decided to do it - no hard algorithmic recursions - just plain Regex with measly one callout function that only fetch and displays named groups/diagnostics.

Obviously there were some challenges - for example the first one was distinguishing between a random sequence of text and an terminated escape sequence.

Currently the last challenge I was faced with was capturing/triggering the callbacks for right-to-left operators in the right order. I have a generic method for this now, for a single operator but it must be extended for more complex cases.
