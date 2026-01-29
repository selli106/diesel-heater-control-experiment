# Contributing to ESPHome Diesel Heater BLE

Thank you for your interest in contributing! This project combines knowledge from multiple heater protocol implementations to provide the best possible ESPHome integration.

## Ways to Contribute

### 🐛 Report Bugs
- Use GitHub Issues
- Include heater model and protocol variant
- Attach debug logs
- Describe expected vs actual behavior

### 📝 Improve Documentation
- Fix typos and clarify instructions
- Add examples for different heater models
- Translate documentation
- Create video tutorials

### 💻 Code Contributions
- Fix bugs
- Add new features
- Improve protocol detection
- Optimize performance

### 🧪 Testing
- Test with different heater models
- Document protocol variants
- Validate sensor accuracy
- Test edge cases

### 💡 Feature Requests
- Describe use case
- Explain expected behavior
- Consider backward compatibility

## Getting Started

### Fork and Clone

```bash
# Fork the repository on GitHub, then:
git clone https://github.com/YOUR-USERNAME/esphome-diesel-heater-ble.git
cd esphome-diesel-heater-ble

# Add upstream remote
git remote add upstream https://github.com/ORIGINAL-REPO/esphome-diesel-heater-ble.git
```

### Development Setup

```bash
# Install ESPHome development environment
pip install esphome

# Create a test configuration
cp examples/basic-config.yaml test-heater.yaml

# Edit with your heater's MAC address
nano test-heater.yaml
```

### Make Changes

```bash
# Create a feature branch
git checkout -b feature/your-feature-name

# Make your changes
# Test thoroughly!

# Commit with clear message
git commit -am "Add feature: description"

# Push to your fork
git push origin feature/your-feature-name
```

### Submit Pull Request

1. Go to your fork on GitHub
2. Click "New Pull Request"
3. Select your feature branch
4. Describe your changes
5. Link related issues
6. Submit!

## Code Standards

### Python Code (Component)

```python
# Follow PEP 8
# Use type hints
# Add docstrings

def my_function(param: int) -> str:
    """
    Brief description.
    
    Args:
        param: Description of parameter
        
    Returns:
        Description of return value
    """
    return str(param)
```

### C++ Code (ESPHome Component)

```cpp
// Follow ESPHome style guide
// Use clear variable names
// Add comments for complex logic

class MyClass {
 public:
  void my_method() {
    // Clear, concise code
  }
  
 protected:
  int my_variable_{0};  // Member variables end with underscore
};
```

### YAML Configuration

```yaml
# Use 2-space indentation
# Add comments for clarity
# Group related items

sensor:
  # Temperature sensors
  - platform: diesel_heater_ble
    cab_temp:
      name: "Room Temperature"
```

## Testing Guidelines

### Before Submitting

- [ ] Code compiles without errors
- [ ] Code compiles without warnings
- [ ] Tested on actual hardware
- [ ] Tested with multiple heater models (if possible)
- [ ] Debug logs clean (no errors)
- [ ] Documentation updated
- [ ] Examples updated (if needed)

### Testing Checklist

```yaml
# Test each function:
- [ ] Heater turns on/off
- [ ] Temperature control works
- [ ] Level control works
- [ ] Sensors report correct values
- [ ] Error handling works
- [ ] Connection recovery works
- [ ] OTA updates work
```

## Documentation Standards

### README Updates

- Keep README concise
- Link to detailed docs
- Update feature list
- Update compatibility list

### Code Documentation

```python
# Module docstring
"""
Module: heater_protocol.py
Description: Handles diesel heater BLE protocol
"""

# Function docstring
def parse_response(data: bytes) -> HeaterState:
    """
    Parse BLE response packet.
    
    Args:
        data: Raw BLE response bytes
        
    Returns:
        Parsed heater state
        
    Raises:
        ValueError: If data is invalid
    """
```

### Commit Messages

```
Format: <type>(<scope>): <subject>

Types:
- feat: New feature
- fix: Bug fix
- docs: Documentation only
- style: Formatting, missing semi-colons, etc.
- refactor: Code restructuring
- test: Adding tests
- chore: Maintenance

Example:
feat(sensor): Add fuel level estimation

- Calculate fuel usage from runtime
- Add fuel level sensor
- Update documentation
```

## Protocol Research

### Adding New Heater Model

1. **Capture BLE Traffic**
   ```bash
   # Use Wireshark with BLE plugin
   # Or Android Bluetooth HCI snoop
   ```

2. **Document Protocol**
   ```markdown
   ## Model: VEVOR XYZ-123
   
   ### Request Format:
   - Header: 0xAA 0x55
   - Password: 0x0C 0x22
   - Commands: (list differences)
   
   ### Response Format:
   - (document structure)
   - (note differences from standard)
   ```

3. **Test Implementation**
   - Create test config
   - Verify all sensors
   - Check all controls
   - Document limitations

4. **Submit Findings**
   - Open issue with model details
   - Attach protocol documentation
   - Include sample packets
   - Note any special behavior

## Review Process

### Pull Request Review

1. **Automated Checks**: CI/CD runs
2. **Code Review**: Maintainer review
3. **Testing**: Request testing if needed
4. **Merge**: Merged when approved

### Review Criteria

- ✅ Code quality
- ✅ Documentation complete
- ✅ Tests pass
- ✅ No breaking changes (or documented)
- ✅ Follows project style
- ✅ Solves stated problem

## Communication

### Channels

- **GitHub Issues**: Bug reports, features
- **GitHub Discussions**: General questions
- **Pull Requests**: Code contributions
- **Email**: Security issues only

### Be Respectful

- Assume good intentions
- Be patient with new contributors
- Provide constructive feedback
- Help others learn

## Recognition

### Contributors

All contributors are recognized:
- Listed in README
- GitHub contributor graph
- Release notes mention

### Significant Contributions

- New heater model support
- Major features
- Extensive documentation
- Long-term maintenance

## Legal

### Licensing

- All contributions under MIT License
- Must not violate other licenses
- Must not violate patents
- Original work or properly attributed

### Copyright

By contributing:
- You grant MIT license rights
- You certify it's your work
- You have rights to contribute
- You understand no warranty

### Protocol Research

- Reverse engineering for interoperability is legal in most jurisdictions
- Document sources clearly
- Respect copyright on documentation
- Don't distribute proprietary firmware

## Getting Help

### New Contributors

- Read documentation first
- Search existing issues
- Ask in Discussions
- Be specific in questions

### Mentorship

Looking for mentors to help with:
- ESPHome component development
- BLE protocol analysis
- Home Assistant integration
- Documentation improvement

## Project Roadmap

### Current Focus
- Improve protocol detection
- Support more heater models
- Better error handling
- Enhanced documentation

### Future Goals
- Climate entity support
- Fuel level monitoring
- Advanced diagnostics
- Multi-heater coordination

## Thank You!

Every contribution helps make this project better for everyone. Whether it's a bug report, documentation fix, or major feature - thank you for your time and effort!

---

**Questions?** Open an issue or discussion on GitHub.

**Need help?** Check the FAQ or ask the community.

**Found a security issue?** See SECURITY.md

---

*Updated: November 2025*
