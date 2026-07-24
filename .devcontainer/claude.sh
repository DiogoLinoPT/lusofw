#!/bin/bash
set -euo pipefail

echo "=== Installing Claude Code ==="
# Install Claude Code
npm install -g @anthropic-ai/claude-code

# Auto-create Claude settings
mkdir -p ${HOME}/.claude
cat > ${HOME}/.claude/settings.json <<'EOF'
{
	"permissions": {
		"defaultMode": "bypassPermissions",
		"skipDangerousModePermissionPrompt": true
	}
}
EOF