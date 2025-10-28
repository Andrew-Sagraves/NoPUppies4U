Sprint 3 — Self Report
Name: Andrew Sagraves
Github ID: Andrew-Sagraves
Group Name: NoPUppies4U

What you planned to do

Implement SUID/SGID binary auditing with package ownership and checksum verification.

Implement systemd unit/timer misconfiguration audit (check ExecStart targets, unit/drop-in ownership and writable dirs).

Keep all checks read-only and safe for auditing.

Add prototypes to the header and integrate the new audits into the codebase.

What you actually did

Implemented suid_package_audit() which:

Enumerates SUID/SGID binaries.

Determines package ownership via dpkg -S or rpm -qf.

Verifies file integrity using package metadata (/var/lib/dpkg/info/*.md5sums or rpm -Vf) and local checksums.

Logs findings and remediation advice to a plaintext log file.

Implemented systemd_unit_audit() which:

Scans common systemd unit directories for .service and .timer files.

Checks unit file ownership and parent directory writability.

Parses ExecStart* lines and flags executables in writable directories, references to /tmp, or non-absolute paths (PATH-hijack risk).

Inspects drop-in override files under /etc/systemd/system/<unit>.d/*.conf for non-root ownership and writable parents.

Logs findings and remediation advice to the same plaintext log file.

Refactored and added compact helper utilities for safe read-only execution (exec_cmd, stat_path, is_writable_by_nonroot, md5, log_line) and produced a concise implementation (replacing the heavily-commented draft).

Added function prototypes and documentation comments to asagrave.h (including the two new audit functions).

Completed the planned tasks — all planned work for this sprint is finished.

What you did not do

None — all planned items were completed.

What problems you encountered

Deciding on robust but simple quoting/escaping for shell-based helpers (e.g., handling paths with single quotes when calling md5sum/dpkg) — resolved by using conservative quoting and documenting the caveat.

Permissions and runtime visibility: find / and some verification commands require root to see everything; tests on non-root showed reduced visibility. Documented behavior and kept functions read-only.

Determining exact locations that should be scanned for drop-ins and unit files across different distributions — mitigated by scanning a standard set of common systemd directories (/etc, /lib, /usr/lib, /run).

Issues you worked on

#28 (referenced earlier) — used as a context pointer for related repository work.

(Integrated new functionality and header prototypes — see commits for filenames and diffs.)

Files you worked on

asagrave.cpp — added suid_package_audit() and systemd_unit_audit() and compact helpers.

asagrave.h — added prototypes and short documentation for the new functions.

Minor touches may have been made to project build files or logging conventions to integrate the new functions (see commit history).

Use of AI and/or 3rd party software

GitHub Copilot — used for small code snippets and autocompletion.

Google Gemini 2.5 — used to help reason about logic and debug small issues.

ChatGPT (assistant) — help designing audit logic, providing compact C++ implementations, and drafting the sprint report.

What you accomplished (summary)

Delivered two new, production-ready, read-only audits that detect high-risk privilege escalation vectors:

suid_package_audit() — finds unowned or tampered SUID/SGID binaries and gives concrete remediation steps.

systemd_unit_audit() — finds misowned systemd units, insecure drop-ins, and risky ExecStart targets.

Added header declarations and consolidated helper functions for easy integration and maintenance.

Kept the implementation lightweight and safe for automated runs (no destructive actions), suitable for inclusion in NoPUppies4U’s next release.
