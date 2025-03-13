import { loginUser } from './../backend/auth.js'; // import the function to test
import bcrypt from 'bcryptjs';
import { supabase } from './../backend/db.js';

// Mocking bcrypt and supabase
jest.mock('bcryptjs');
jest.mock('./../backend/db.js');

describe('Login Tests', () => {

  // Test successful login
  it('should return a token for valid credentials', async () => {
    const mockUser = {
      id: 1,
      email: 'test@example.com',
      password: '$2a$10$WwzF.DVpJahOQJ0v9M9eFJ7UyfhBC1jXok9mST7AzzgJ5ih5My8pu', // example hashed password
    };

    // Mock supabase query to return the mock user
    supabase.from.mockReturnValue({
      select: jest.fn().mockReturnValue({
        eq: jest.fn().mockReturnValue({
          single: jest.fn().mockResolvedValue({ data: [mockUser], error: null })
        })
      })
    });

    // Mock bcrypt.compare to return true
    bcrypt.compare.mockResolvedValue(true);

    const result = await loginUser('test@example.com', 'password123');

    expect(result).toHaveProperty('token');
    expect(result).toHaveProperty('user');
    expect(result.user.email).toBe('test@example.com');
  });

  // Test invalid credentials (wrong password)
  it('should return null for invalid password', async () => {
    const mockUser = {
      id: 1,
      email: 'test@example.com',
      password: '$2a$10$WwzF.DVpJahOQJ0v9M9eFJ7UyfhBC1jXok9mST7AzzgJ5ih5My8pu',
    };

    supabase.from.mockReturnValue({
      select: jest.fn().mockReturnValue({
        eq: jest.fn().mockReturnValue({
          single: jest.fn().mockResolvedValue({ data: [mockUser], error: null })
        })
      })
    });

    bcrypt.compare.mockResolvedValue(false); // Password doesn't match

    const result = await loginUser('test@example.com', 'wrongpassword');

    expect(result).toBeNull(); // Invalid login should return null
  });

  // Test user not found
  it('should return null if user does not exist', async () => {
    supabase.from.mockReturnValue({
      select: jest.fn().mockReturnValue({
        eq: jest.fn().mockReturnValue({
          single: jest.fn().mockResolvedValue({ data: [], error: null })
        })
      })
    });

    const result = await loginUser('nonexistent@example.com', 'password123');
    
    expect(result).toBeNull(); // No user found, should return null
  });

  // Test error while querying the database
  it('should return null if there is an error while fetching user', async () => {
    supabase.from.mockReturnValue({
      select: jest.fn().mockReturnValue({
        eq: jest.fn().mockReturnValue({
          single: jest.fn().mockResolvedValue({ data: null, error: { message: 'Some error' } })
        })
      })
    });

    const result = await loginUser('test@example.com', 'password123');
    
    expect(result).toBeNull(); // Should return null if there's an error
  });
});