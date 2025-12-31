# Open Source Contribution Workflow

## One-time setup

1. Fork the original repo on GitHub (creates your-username/repo)
2. Clone your fork locally: `git clone git@github.com:your-username/repo.git`
3. Add upstream remote: `git remote add upstream git@github.com:original-owner/repo.git`

## For each contribution

1. Sync with upstream:

```bash
git checkout master
git fetch upstream
git merge upstream/master
```

2. Create feature branch: `git checkout -b bugfix/descriptive-name`
3. Make changes, commit
4. Push to your fork: `git push origin bugfix/descriptive-name`
5. Create PR on GitHub: your fork->original repo

## Keeping PR updated (if maintainer requests changes)

1. Make changes locally on the same branch
2. Commit
3. `git push origin bugfix/descriptive-name`
4. PR updates automatically

## After PR merged

1. `git checkout master`
2. `git fetch upstream`
3. `git merge upstream/master`
4. `git push origin master` (sync your fork)
5. Delete branch: `git branch -d bugfix/descriptive-name`

## Useful commands

- `git remote -v` - see all remotes
- `git fetch upstream` - get latest from original repo
- `git rebase upstream/master` - rebase your branch on latest (alternative to merge)

## Useful PR names and tags

```bash
feature/    - New features or enhancements

bugfix/     - Bug fixes
hotfix/     - Urgent production fixes
chore/      - Maintenance, dependencies, tooling
refactor/   - Code restructuring without behavior change
docs/       - Documentation only
test/       - Adding or updating tests
perf/       - Performance improvements
style/      - Formatting, linting (no logic change)
ci/         - CI/CD pipeline changes
build/      - Build system, configuration
release/    - Release preparation
experiment/ - Experimental/prototype work
```

### Common pattern: `prefix/issue-number-short-description`

### Examples

```bash
feature/123-add-user-auth
bugfix/456-fix-parser-crash
hotfix/789-security-patch
refactor/cleanup-error-handling
```

**Most teams use: `feature/`, `bugfix/`, `hotfix`, `chore/` as the core set.**
