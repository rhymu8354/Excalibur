#pragma once

/**
 * @file HexDumpNetworkConnectionDecorator.hpp
 *
 * This module declares the HexDumpNetworkConnectionDecorator class.
 *
 * © 2019 by Richard Walters
 */

#include <functional>
#include <memory>
#include <string>
#include <SystemAbstractions/INetworkConnection.hpp>

/**
 * This is a decorator for SystemAbstractions::INetworkConnection which
 * outputs hex dumps about all data that passes through the connection.
 */
class HexDumpNetworkConnectionDecorator
    : public SystemAbstractions::INetworkConnection
{
// Types
public:
    /**
     * This is the type of function used to publish a line of a hex dump from
     * the decorator.
     *
     * @param[in] line
     *     This is the line of hex dump published by the decorator.
     */
    typedef std::function<
        void(const std::string& line)
    > HexDumpDelegate;

    // Lifecycle management
public:
    ~HexDumpNetworkConnectionDecorator() noexcept;
    HexDumpNetworkConnectionDecorator(const HexDumpNetworkConnectionDecorator&) = delete;
    HexDumpNetworkConnectionDecorator(HexDumpNetworkConnectionDecorator&&) noexcept;
    HexDumpNetworkConnectionDecorator& operator=(const HexDumpNetworkConnectionDecorator&) = delete;
    HexDumpNetworkConnectionDecorator& operator=(HexDumpNetworkConnectionDecorator&&) noexcept;

    // Public Methods
public:
    /**
     * This is the default constructor.
     */
    HexDumpNetworkConnectionDecorator();

    /**
     * This method sets up the decorator with the network connection to
     * decorate and the delegate to call to deliver hex dump lines.
     *
     * @param[in] lowerLayer
     *     This is the lower-level connection to decorate.
     *
     * @param[in] hexDumpDelegate
     *     This is the function to call whenever a hex dump is produced
     *     by the decorator.
     */
    void Decorate(
        std::shared_ptr< SystemAbstractions::INetworkConnection > lowerLayer,
        HexDumpDelegate hexDumpDelegate
    );

    // SystemAbstractions::INetworkConnection
public:
    virtual SystemAbstractions::DiagnosticsSender::UnsubscribeDelegate SubscribeToDiagnostics(
        SystemAbstractions::DiagnosticsSender::DiagnosticMessageDelegate delegate,
        size_t minLevel = 0
    ) override;
    virtual bool Connect(uint32_t peerAddress, uint16_t peerPort) override;
    virtual bool Process(
        MessageReceivedDelegate messageReceivedDelegate,
        BrokenDelegate brokenDelegate
    ) override;
    virtual uint32_t GetPeerAddress() const override;
    virtual uint16_t GetPeerPort() const override;
    virtual bool IsConnected() const override;
    virtual uint32_t GetBoundAddress() const override;
    virtual uint16_t GetBoundPort() const override;
    virtual void SendMessage(const std::vector< uint8_t >& message) override;
    virtual void Close(bool clean = false) override;

    // Private Properties
private:
    /**
     * This is the type of structure that contains the private
     * properties of the instance.  It is defined in the implementation
     * and declared here to ensure that it is scoped inside the class.
     */
    struct Impl;

    /**
     * This contains the private properties of the instance.
     */
    std::shared_ptr< Impl > impl_;
};
