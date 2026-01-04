# Repository Lockdown Checklist

Use this checklist for each project you want to protect and monetize.

---

## Project: ___________________

### Phase 1: Preparation (BEFORE going private)

- [ ] **Document existing users** who get free access
  - List all GitHub usernames of friends/followers
  - Save to a private file (don't commit to repo)
  - You'll need these to add as collaborators

- [ ] **Choose pricing strategy**
  - [ ] Single price: $___
  - [ ] Tiered pricing: Basic $___ / Pro $___/ Enterprise $___
  - [ ] Bundle with other projects: $___

- [ ] **Set up sales platform**
  - Platform choice: ☐ Gumroad  ☐ Ko-fi  ☐ GitHub Sponsors  ☐ PayPal
  - Create product listing
  - Add form field for "GitHub username"
  - Test purchase flow

- [ ] **Create essential files**
  - [ ] Add LICENSE file (use template, fill in [PLACEHOLDERS])
  - [ ] Update README with pricing header (use template)
  - [ ] Add CONTRIBUTING.md if accepting contributions
  - [ ] Create .github/FUNDING.yml for sponsor button

### Phase 2: Repository Configuration

- [ ] **Update README.md**
  - [ ] Add "PREMIUM SOFTWARE" header with price
  - [ ] Add purchase link
  - [ ] Add complimentary access note
  - [ ] Update installation instructions
  - [ ] Add license summary

- [ ] **Update LICENSE file**
  - [ ] Replace [PROJECT_NAME] with actual name
  - [ ] Replace [PRICE] with your price
  - [ ] Replace [VERSION] with current version
  - [ ] Replace [SALES_LINK] with your link
  - [ ] Replace [REPO_NAME] with repository name

- [ ] **Add GitHub Funding**

  ```yaml
  # .github/FUNDING.yml
  github: [Crashkeyz]
  custom: ["https://your-gumroad-link", "https://your-kofi"]
  ```

- [ ] **Configure repository settings**
  - Go to Settings → General → Features
  - [ ] Disable: Wikis (unless needed)
  - [ ] Disable: Projects (unless needed)
  - [ ] Enable: Issues (for licensed users)
  - [ ] Enable: Discussions (optional for community)
  - Go to Settings → Branches
  - [ ] Protect main branch (require PR reviews)

### Phase 3: Add Complimentary Users

- [ ] **Before making private, add all free users**
  - Settings → Collaborators → Add people
  - Add each username from your list
  - Set permission: "Read" (they can clone but not push)
  - Send them a heads up email/message

- [ ] **Verify they have access**
  - Ask one friend to confirm they can see repo
  - Fix any access issues NOW

### Phase 4: Going Private

- [ ] **Final commit**

  ```bash
  git add .
  git commit -m "Prepare for private release - v[VERSION]"
  git push
  ```

- [ ] **Make repository private**
  - Settings → General → Danger Zone
  - "Change visibility" → "Make private"
  - Type repository name to confirm
  - Click "I understand, make this repository private"

- [ ] **Verify privacy**
  - Log out of GitHub
  - Try to access repository URL
  - Should see "404 Not Found"

### Phase 5: Launch

- [ ] **Announce on social media**
  - [ ] Twitter/X post
  - [ ] Reddit (relevant subreddits)
  - [ ] Discord servers
  - [ ] Dev.to or Hashnode blog post
  - [ ] LinkedIn (if professional)

- [ ] **Update sales page**
  - [ ] Final price check
  - [ ] Add demo screenshots/video
  - [ ] Verify purchase flow works
  - [ ] Test "add collaborator" process

- [ ] **Email existing users**

  ```
  Subject: [PROJECT_NAME] is now a premium product
  
  Hey [Name],
  
  I've just launched [PROJECT_NAME] as a paid product ($[PRICE]).
  
  As an existing friend/follower, you've been given complimentary 
  access! The repository is now private, but you should still have 
  access to clone and update.
  
  Thank you for your support!
  
  - Crashkeyz
  ```

### Phase 6: Post-Launch

- [ ] **Process purchases within 2 hours**
  - Get their GitHub username
  - Add as collaborator with "Read" permission
  - Send confirmation email with repo link
  - Thank them for purchase

- [ ] **Respond to all inquiries**
  - Check GitHub Issues daily
  - Respond to emails within 24 hours
  - Be helpful and professional

- [ ] **Track metrics**
  - Sales count: ___
  - Revenue (after fees): $___
  - Support requests: ___
  - Positive feedback: ___
  - Refund requests: ___

- [ ] **Gather testimonials**
  - Ask happy customers for quotes
  - Add to README (with permission)
  - Use in marketing materials

### Phase 7: Maintenance

- [ ] **Regular updates**
  - [ ] Bug fixes within 48 hours (critical)
  - [ ] Feature updates monthly
  - [ ] Changelog maintained
  - [ ] Version tags on releases

- [ ] **Community engagement**
  - [ ] Check Issues daily
  - [ ] Merge valuable PRs
  - [ ] Credit contributors
  - [ ] Share user projects

---

## Pricing Recommendations by Project Type

### Simple Utility/Tool

- **$3-5**: Quick scripts, converters, generators
- **Complexity**: Low
- **Support needed**: Minimal

### Medium Application

- **$10-15**: Apps with UI, multiple features
- **Complexity**: Medium
- **Support needed**: Moderate

### Complex Framework/System

- **$20-50**: Full frameworks, game engines, complete systems
- **Complexity**: High
- **Support needed**: High

### Professional/Enterprise Tool

- **$50-200+**: Professional use, saves significant time/money
- **Complexity**: Very high
- **Support needed**: Very high + consulting

---

## Quick Reference: Making Repos Private

```bash
# Via GitHub CLI (faster for multiple repos)
gh repo edit Crashkeyz/REPO_NAME --visibility private

# Or via web interface:
# github.com/Crashkeyz/REPO_NAME/settings → Danger Zone → Change visibility
```

---

## Bulk Operations Script

Save this as `lockdown_repos.sh`:

```bash
#!/bin/bash

# List of repositories to lock down
REPOS=(
  "project1"
  "project2"
  "project3"
)

for repo in "${REPOS[@]}"; do
  echo "Processing $repo..."
  
  # Clone if not exists
  if [ ! -d "$repo" ]; then
    gh repo clone Crashkeyz/$repo
  fi
  
  cd $repo
  
  # Copy templates
  cp ../UltimatumCardputerKey/TEMPLATES/LICENSE_TEMPLATE.md ./LICENSE
  
  # Commit
  git add LICENSE
  git commit -m "Add proprietary license"
  git push
  
  # Make private (comment out until ready)
  # gh repo edit Crashkeyz/$repo --visibility private
  
  cd ..
  echo "$repo done!"
done

echo "All repositories processed!"
```

---

## Support Email Template

Save for quick responses:

```
Subject: Re: [PROJECT_NAME] Purchase

Hi [Name],

Thank you for purchasing [PROJECT_NAME]!

I've added you as a collaborator to the private repository:
https://github.com/Crashkeyz/[REPO_NAME]

You now have access to:
✅ Full source code
✅ Clone and pull updates anytime
✅ Lifetime updates for v[VERSION].x
✅ Priority support via GitHub Issues

Getting started:
1. Clone: git clone https://github.com/Crashkeyz/[REPO_NAME]
2. Follow README.md for setup
3. Check CHANGELOG.md for latest updates

Need help? Open an issue in the repository or reply to this email.

Thanks for supporting open-source development!

- Crashkeyz
```

---

## Legal Note

**I am not a lawyer.** This checklist is for guidance only. Consider:

- Consulting a lawyer for your jurisdiction
- Understanding tax implications of income
- Registering as a business (if needed)
- Having terms of service and privacy policy
- Understanding export controls (if applicable)

---

## Resources

- **Payment Platforms**:
  - Gumroad: <https://gumroad.com> (10% fee)
  - Ko-fi: <https://ko-fi.com> (5% fee)
  - GitHub Sponsors: <https://github.com/sponsors> (0% fee)

- **Legal Templates**:
  - Terms of Service: <https://www.termsfeed.com>
  - Privacy Policy: <https://www.privacypolicies.com>

- **Marketing**:
  - Product Hunt: <https://producthunt.com>
  - Hacker News: <https://news.ycombinator.com>
  - Reddit: r/SideProject, r/EntrepreneurRideAlong

---

**Good luck with your projects! 🚀**
