/**
 * @file HexDumpNetworkConnectionDecorator.cpp
 *
 * This module contains the implementation of the
 * HexDumpNetworkConnectionDecorator class.
 *
 * © 2019 by Richard Walters
 */

#include "HexDumpNetworkConnectionDecorator.hpp"

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <SystemAbstractions/StringExtensions.hpp>
#include <vector>

struct HexDumpNetworkConnectionDecorator::Impl {
    // Properties

    /**
     * This is the function to call whenever a line of a hex dump
     * is published by the decorator.
     */
    HexDumpDelegate hexDumpDelegate;

    /**
     * This is the interface to the network connection being decorated.
     */
    std::shared_ptr< SystemAbstractions::INetworkConnection > lowerLayer;

    // Methods

    /**
     * Publish hex dump lines to show the contents of the given
     * vector of data, in the form of hexadecimal and ASCII.
     *
     * @param[in] data
     *     This is the data to show in the hex dump.
     *     This is the number of bytes to show in the hex dump.
     */
    void HexDump(const std::vector< uint8_t >& data) {
        std::ostringstream l, r, c;
        for (size_t i = 0; i < data.size(); ++i) {
            if ((i % 16) == 0) {
                l = std::ostringstream();
                r = std::ostringstream();
                c = std::ostringstream();
                l << std::hex << std::setw(4) << std::setfill('0') << i;
            }
            if ((i % 8) == 0) {
                l << " ";
            }
            l << " " << std::hex << std::setw(2) << std::setfill('0') << (int)(uint8_t)data[i];
            if (
                (data[i] > 32)
                && (data[i] < 127)
            ) {
                r << (char)data[i];
            } else {
                r << '.';
            }
            if (
                (((i + 1) % 16) == 0)
                || (i + 1 == data.size())
            ) {
                for (size_t j = 0; ((i + j) % 16) != 0; ++j) {
                    if (((i + j) % 8) == 0) {
                        l << " ";
                    }
                    l << "   ";
                }
                c << l.str() << "  " << r.str();
                hexDumpDelegate(c.str());
            }
        }
    }

};

HexDumpNetworkConnectionDecorator::~HexDumpNetworkConnectionDecorator() noexcept = default;
HexDumpNetworkConnectionDecorator::HexDumpNetworkConnectionDecorator(HexDumpNetworkConnectionDecorator&&) noexcept = default;
HexDumpNetworkConnectionDecorator& HexDumpNetworkConnectionDecorator::operator=(HexDumpNetworkConnectionDecorator&&) noexcept = default;

HexDumpNetworkConnectionDecorator::HexDumpNetworkConnectionDecorator()
    : impl_(new Impl())
{
}

void HexDumpNetworkConnectionDecorator::Decorate(
    std::shared_ptr< SystemAbstractions::INetworkConnection > lowerLayer,
    HexDumpDelegate hexDumpDelegate
) {
    impl_->lowerLayer = lowerLayer;
    impl_->hexDumpDelegate = hexDumpDelegate;
}

SystemAbstractions::DiagnosticsSender::UnsubscribeDelegate HexDumpNetworkConnectionDecorator::SubscribeToDiagnostics(
    SystemAbstractions::DiagnosticsSender::DiagnosticMessageDelegate delegate,
    size_t minLevel
) {
    return impl_->lowerLayer->SubscribeToDiagnostics(delegate, minLevel);
}

bool HexDumpNetworkConnectionDecorator::Connect(uint32_t peerAddress, uint16_t peerPort) {
    return impl_->lowerLayer->Connect(peerAddress, peerPort);
}

bool HexDumpNetworkConnectionDecorator::Process(
    MessageReceivedDelegate messageReceivedDelegate,
    BrokenDelegate brokenDelegate
) {
    const std::weak_ptr< Impl > implWeak(impl_);
    const auto decoratedMessageReceivedDelegate = [
        implWeak,
        messageReceivedDelegate
    ](const std::vector< uint8_t >& message){
        const auto impl(implWeak.lock());
        if (impl == nullptr) {
            return;
        }
        impl->hexDumpDelegate(
            SystemAbstractions::sprintf(
                "Received %zu bytes:",
                message.size()
            )
        );
        impl->HexDump(message);
        messageReceivedDelegate(message);
    };
    return impl_->lowerLayer->Process(decoratedMessageReceivedDelegate, brokenDelegate);
}

uint32_t HexDumpNetworkConnectionDecorator::GetPeerAddress() const {
    return impl_->lowerLayer->GetPeerAddress();
}

uint16_t HexDumpNetworkConnectionDecorator::GetPeerPort() const {
    return impl_->lowerLayer->GetPeerPort();
}

bool HexDumpNetworkConnectionDecorator::IsConnected() const {
    return impl_->lowerLayer->IsConnected();
}

uint32_t HexDumpNetworkConnectionDecorator::GetBoundAddress() const {
    return impl_->lowerLayer->GetBoundAddress();
}

uint16_t HexDumpNetworkConnectionDecorator::GetBoundPort() const {
    return impl_->lowerLayer->GetBoundPort();
}

void HexDumpNetworkConnectionDecorator::SendMessage(const std::vector< uint8_t >& message) {
    impl_->hexDumpDelegate(
        SystemAbstractions::sprintf(
            "Sending %zu bytes:",
            message.size()
        )
    );
    impl_->HexDump(message);
    impl_->lowerLayer->SendMessage(message);
}

void HexDumpNetworkConnectionDecorator::Close(bool clean) {
    impl_->lowerLayer->Close(clean);
}
