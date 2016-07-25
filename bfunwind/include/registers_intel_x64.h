//
// Bareflank Unwind Library
//
// Copyright (C) 2015 Assured Information Security, Inc.
// Author: Rian Quinn        <quinnr@ainfosec.com>
// Author: Brendan Kerrigan  <kerriganb@ainfosec.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef REGISTERS_INTEL_X64_H
#define REGISTERS_INTEL_X64_H

#include <log.h>
#include <abort.h>
#include <registers.h>

#if (MAX_NUM_REGISTERS < 17)
#error MAX_NUM_REGISTERS was set too low
#endif

// -----------------------------------------------------------------------------
// Load / Store Registers
// -----------------------------------------------------------------------------

struct registers_intel_x64_t
{
    uint64_t rax;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r08;
    uint64_t r09;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rip;
};

/// __store_registers_intel_x64
///
/// Stores the state of the registers into a structure
///
/// @param state the register state to store state too
/// @return always returns 0
///
extern "C"
void __store_registers_intel_x64(registers_intel_x64_t *state);

/// __load_registers_intel_x64
///
/// Restores the state of the registers from a structure
///
/// @param state the register state to store state too
/// @return always returns 0
///
extern "C"
void __load_registers_intel_x64(registers_intel_x64_t *state);

// -----------------------------------------------------------------------------
// Register State
// -----------------------------------------------------------------------------
//
// Defines the register state for x86_64. The only unexpected thing here is
// the System V 64bit ABI defines the register order as rax, rdx, rcx, rbx, and
// not rax, rbx, rcx, rdx. This makes a really big difference because the
// reg(1) used by the personality function is stored in rdx as a result.
//
// See register.h for more information
//

class register_state_intel_x64 : public register_state
{
public:
    register_state_intel_x64(registers_intel_x64_t registers)
    {
        m_registers = registers;
        m_tmp_registers = registers;
    }

    virtual ~register_state_intel_x64() {}

    uint64_t get_ip() const override
    { return m_registers.rip; }

    register_state &set_ip(uint64_t value) override
    {
        m_tmp_registers.rip = value;
        return *this;
    }

    uint64_t get(uint64_t index) const override
    {
        if (index >= max_num_registers())
            ABORT("register index out of bounds");

        return ((uint64_t *)&m_registers)[index];
    }

    register_state &set(uint64_t index, uint64_t value) override
    {
        if (index >= max_num_registers())
            ABORT("register index out of bounds");

        ((uint64_t *)&m_tmp_registers)[index] = value;

        return *this;
    }

    void commit() override
    { m_registers = m_tmp_registers; }

    void commit(uint64_t cfa) override
    {
        m_tmp_registers.rsp = cfa;
        commit();
    }

    void resume() override
    { __load_registers_intel_x64(&m_registers); }

    uint64_t max_num_registers() const override
    { return 17; }

    const char *name(uint64_t index) const override
    {
        if (index >= max_num_registers())
            ABORT("register index out of bounds");

        switch (index)
        {
            case 0x00: return "rax";
            case 0x01: return "rdx";
            case 0x02: return "rcx";
            case 0x03: return "rbx";
            case 0x04: return "rdi";
            case 0x05: return "rsi";
            case 0x06: return "rbp";
            case 0x07: return "rsp";
            case 0x08: return "r08";
            case 0x09: return "r09";
            case 0x0A: return "r10";
            case 0x0B: return "r11";
            case 0x0C: return "r12";
            case 0x0D: return "r13";
            case 0x0E: return "r14";
            case 0x0F: return "r15";
            case 0x10: return "rip";
            default: return "";
        }
    }

    void dump() const override
    {
        uint64_t *rsp = (uint64_t *)m_registers.rsp;

        debug("Register State:\n")
        debug("  rax: 0x%08lx\n", m_registers.rax);
        debug("  rdx: 0x%08lx\n", m_registers.rdx);
        debug("  rcx: 0x%08lx\n", m_registers.rcx);
        debug("  rbx: 0x%08lx\n", m_registers.rbx);
        debug("  rdi: 0x%08lx\n", m_registers.rdi);
        debug("  rsi: 0x%08lx\n", m_registers.rsi);
        debug("  rbp: 0x%08lx\n", m_registers.rbp);
        debug("  rsp: 0x%08lx\n", m_registers.rsp);
        debug("  r08: 0x%08lx\n", m_registers.r08);
        debug("  r09: 0x%08lx\n", m_registers.r09);
        debug("  r10: 0x%08lx\n", m_registers.r10);
        debug("  r11: 0x%08lx\n", m_registers.r11);
        debug("  r12: 0x%08lx\n", m_registers.r12);
        debug("  r13: 0x%08lx\n", m_registers.r13);
        debug("  r14: 0x%08lx\n", m_registers.r14);
        debug("  r15: 0x%08lx\n", m_registers.r15);
        debug("  rip: 0x%08lx\n", m_registers.rip);
        debug("\n")

        debug("Stack State:\n")
        debug("  rsp[-8]: %p\n", (void *)rsp[-8]);
        debug("  rsp[-7]: %p\n", (void *)rsp[-7]);
        debug("  rsp[-6]: %p\n", (void *)rsp[-6]);
        debug("  rsp[-5]: %p\n", (void *)rsp[-5]);
        debug("  rsp[-4]: %p\n", (void *)rsp[-4]);
        debug("  rsp[-3]: %p\n", (void *)rsp[-3]);
        debug("  rsp[-2]: %p\n", (void *)rsp[-2]);
        debug("  rsp[-1]: %p\n", (void *)rsp[-1]);
        debug("  rsp[0] : %p\n", (void *)rsp[0]);
        debug("  rsp[1] : %p\n", (void *)rsp[1]);
        debug("  rsp[2] : %p\n", (void *)rsp[2]);
        debug("  rsp[3] : %p\n", (void *)rsp[3]);
        debug("  rsp[4] : %p\n", (void *)rsp[4]);
        debug("  rsp[5] : %p\n", (void *)rsp[5]);
        debug("  rsp[6] : %p\n", (void *)rsp[6]);
        debug("  rsp[7] : %p\n", (void *)rsp[7]);
        debug("  rsp[8] : %p\n", (void *)rsp[8]);
        debug("\n")
    }

private:
    registers_intel_x64_t m_registers;
    registers_intel_x64_t m_tmp_registers;
};

#endif
