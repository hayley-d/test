import { registerUser } from '../../backend/auth.js';  // Assuming correct import

describe('Registration Tests', () => {
  
  it('should return error for invalid email format', async () => {
    const result = await registerUser('John', 'Doe', '1234567890', 'invalid-email', 'Password1!');
    expect(result.error).toBe('Invalid email');
  });
  
  it('should return error for invalid password', async () => {
    const result = await registerUser('John', 'Doe', '1234567890', 'test@example.com', 'short');
    expect(result.error).toBe('Password must be at least 8 characters long.');
  });
  
  it('should register the user with valid data', async () => {
    const result = await registerUser('John', 'Doe', '1234567890', 'test@example.com', 'Password1!');
    expect(result).toHaveProperty('name', 'John');
    expect(result).toHaveProperty('email', 'test@example.com');
  });

  it('should return error if email already exists', async () => {
    // Register the user first
    await registerUser('Jane', 'Doe', '0987654321', 'jane@example.com', 'Password1!');
    
    // Try to register with the same email again
    const result = await registerUser('Jane', 'Doe', '0987654321', 'jane@example.com', 'Password1!');
    expect(result.error).toBe('Email already exists');
  });
  
  it('should return error for missing required fields', async () => {
    const result = await registerUser('Jane', '', '0987654321', '', 'Password1!');
    expect(result.error).toBe('Missing required fields');
  });
  
});