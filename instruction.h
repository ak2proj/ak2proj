#pragma once

enum class instructions
{
    exit, 
    read, 
    write,
    load, 
    load_paging_register
};

class operand
{
public:
    operand() = default;
    operand(const operand &) = default;
    
    operand(bool is_register, std::uintmax_t value) : _is_valid{ true }, _is_register{ is_register }, _value{ value }
    {
    }
    
    bool is_valid() const
    {
        return _is_valid;
    }
    
    bool is_register() const
    {
        _check_valid();
        return _is_register;
    }
    
    std::uintmax_t register_index() const
    {
        _check_valid();
        
        if (!_is_register)
        {
            throw std::runtime_error{ "tried to use a value operand as a register operand" };
        }
        
        return _value;
    }
    
    std::uintmax_t value() const
    {
        _check_valid();
        
        if (_is_register)
        {
            throw std::runtime_error{ "tried to use a register operand as a value operand" };
        }
        
        return _value;
    }
    
private:
    void _check_valid() const
    {
        if (!_is_valid)
        {
            throw std::runtime_error{ "tried to use an invalid operand" };
        }
    }
    
    bool _is_valid = false;
    bool _is_register;
    std::uintmax_t _value;
};

class instruction
{
public:
    instruction() : _type{ instructions::exit }
    {
    }
    
    instruction(instructions type, std::uintmax_t length, operand destination, operand source) : _type{ type }, _length{ length }, _destination{ destination }, 
        _source{ source }
    {
    }
    
    instructions type() const
    {
        return _type;
    }
    
    std::uintmax_t length() const
    {
        return _length;
    }
    
    const operand & destination_operand() const
    {
        return _destination;
    }
    
    const operand & source_operand() const
    {
        return _source;
    }
    
private:
    instructions _type;
    std::uintmax_t _length;
    operand _destination;
    operand _source;
};
